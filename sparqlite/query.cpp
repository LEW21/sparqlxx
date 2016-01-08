#include "database.h"

#include "../algebratransformers/to_quads.h"
#include "../algebratransformers/merge_joins.h"
#include "../algebratransformers/merge_basics.h"
#include "algebra.h"

namespace sparqlite
{
	using namespace Algebra;

	struct Dataset
	{
		Database* database;
		sparqlxx::Dataset dataset;

		Dataset(Database* database, sparqlxx::Dataset dataset): database(database), dataset(dataset) {}

		auto match(const sparqlxx::QuadsVP& p) const -> sparqlxx::Solutions
		{
			// TODO
			return database->match(p);
		}
	};

	inline auto filter(const std::vector<sparqlxx::Constraint>&, sparqlxx::Solutions&& s, Dataset) -> sparqlxx::Solutions
	{
		return s; // TODO
	}

	inline auto eval(const Dataset&, const sparqlxx::Algebra::AnyOp&) -> sparqlxx::Solutions;

	inline auto eval(const Dataset&, sparqlxx::Algebra::Null) -> sparqlxx::Solutions
	{
		return sparqlxx::Solutions{};
	}

	inline auto eval(const Dataset&, const sparqlxx::Algebra::Basic&) -> sparqlxx::Solutions {std::terminate();} // impossible after to_quads.

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Quad& q) -> sparqlxx::Solutions
	{
		return D.match(q.quads);
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Filter& op) -> sparqlxx::Solutions
	{
		return filter(op.constraints, eval(D, op.data), D);
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Join& op) -> sparqlxx::Solutions
	{
		auto sols = std::vector<Solutions>{};
		sols.reserve(op.ops.size());
		for (const auto& subop : op.ops)
			sols.emplace_back(eval(D, subop));
		return join(std::move(sols));
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::LeftJoin& op) -> sparqlxx::Solutions
	{
		return leftjoin(eval(D, op.a), eval(D, op.b)); // TODO Filter
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Minus& op) -> sparqlxx::Solutions
	{
		return minus(eval(D, op.a), eval(D, op.b));
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Union& op) -> sparqlxx::Solutions
	{
		return union_(eval(D, op.a), eval(D, op.b));
	}

	inline auto eval(const Dataset&, const sparqlxx::Algebra::Graph&) -> sparqlxx::Solutions {std::terminate();} // impossible after to_quads.

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Extend& op) -> sparqlxx::Solutions
	{
		return extend(eval(D, op.data), op.var, op.exp);
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Distinct& op) -> sparqlxx::Solutions
	{
		return distinct(eval(D, op.data));
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Reduced& op) -> sparqlxx::Solutions
	{
		return reduced(eval(D, op.data));
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Project& op) -> sparqlxx::Solutions
	{
		return project(eval(D, op.data), op.vars);
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::Slice& op) -> sparqlxx::Solutions
	{
		return slice(eval(D, op.data), op.start, op.length);
	}

	inline auto eval(const Dataset&, const sparqlxx::Algebra::Service&) -> sparqlxx::Solutions
	{
		throw std::invalid_argument("SERVICE is not yet supported.");
	}

	inline auto eval(const Dataset& D, const sparqlxx::Algebra::AnyOp& op) -> sparqlxx::Solutions
	{
		return op->call([&D](const auto& op){return eval(D, op);});
	}

	auto Database::match(const sparqlxx::QuadsVP& patterns) const -> sparqlxx::Solutions
	{
		auto solutions = std::vector<sparqlxx::Solutions>{};
		solutions.reserve(patterns.size());
		for (auto p : patterns)
		{
			auto S = sparqlxx::Solutions{};

			auto mv = MapVars{};
			mv.add_var(p.subject);
			mv.add_var(p.predicate);
			mv.add_var(p.object);
			mv.add_var(p.graph);
			S.vars = mv.vars;

			try
			{
				auto rows = stmt.match(Quad{res[p.subject], res[p.predicate], res[p.object], res[p.graph]});
				S.rows.reserve(rows.size());
				for (auto r : rows)
				{
					auto q = res.toQuad(r);
					try
					{
						mv.add_term(q.subject);
						mv.add_term(q.predicate);
						mv.add_term(q.object);
						mv.add_term(q.graph);
						S.rows.emplace_back(mv.take_row());
					}
					catch (Break&) {}
					mv.reset();
				}
			}
			catch (std::out_of_range&) // thrown by res[p.*]
			{
				// Nothing can get matched.
			}

			solutions.emplace_back(std::move(S));
		}
		return join(std::move(solutions));
	}

	template <>
	auto Database::_query<sparqlxx::Select>(const sparqlxx::Select& s) -> sparqlxx::Solutions
	{
		auto op = s.op;

		if (!op)
			return {};

		using namespace sparqlxx::AlgebraTransformers;
		*op = merge_basics(merge_joins(to_quads(std::move(*op), sparqlxx::Iri{RIRI::DefaultGraphNode})));

		return eval(Dataset{this, s.dataset}, op);
	}

	template <>
	auto Database::_query<sparqlxx::Describe>(const sparqlxx::Describe&) -> sparqlxx::Triples
	{
		auto out = sparqlxx::Triples{};
		for (auto s : stmt.all)
			out.emplace_back(res.toTriple(s));
		return out;
	}
}
