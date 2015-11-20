#include "database.h"

namespace sparqlite
{
	struct Break {};

	struct MapVar
	{
		enum class Op { Ignore, Append, Check } op;
		int row_pos;

		MapVar(Op op, int row_pos): op(op), row_pos(row_pos) {}
	};

	struct MapVars
	{
		std::vector<MapVar> map;
		std::unordered_map<std::string, int> dedup;
		std::vector<sparqlxx::Var> vars;
		std::vector<sparqlxx::Term> nr;
		int i = 0;

		void add_var(const sparqlxx::Var& v)
		{
			if (dedup.count(v.name))
			{
				map.emplace_back(MapVar::Op::Check, dedup[v.name]);
			}
			else
			{
				dedup[v.name] = vars.size();
				map.emplace_back(MapVar::Op::Append, -1);
				vars.emplace_back(std::move(v));
			}
		}

		template <typename VT>
		void add_var(const VT& vt)
		{
			if (!vt.template is<sparqlxx::Var>())
			{
				map.emplace_back(MapVar::Op::Ignore, -1);
				return;
			}

			add_var(vt.template get<sparqlxx::Var>());
		}

		void add_term(const sparqlxx::Term& t)
		{
			auto mv = map[i];
			switch (mv.op)
			{
				case MapVar::Op::Ignore:
					break;

				case MapVar::Op::Append:
					nr.emplace_back(t);
					break;

				case MapVar::Op::Check:
					if (nr[mv.row_pos] != t)
						throw Break{};
					break;
			}
			++i;
		}

		auto take_row() -> std::vector<sparqlxx::Term>
		{
			auto row = std::move(nr);
			nr = {};
			nr.reserve(vars.size());
			i = 0;
			assert(row.size() == vars.size());
			return row;
		}
	};

	inline auto join(std::vector<sparqlxx::Solutions>&& solutions) -> sparqlxx::Solutions
	{
		auto S = solutions[0];

		for (int i = 1; i < solutions.size(); ++i)
		{
			auto s1 = std::move(S);
			auto s2 = solutions[i];
			S = sparqlxx::Solutions();

			auto mv = MapVars{};
			for (const auto& v : s1.vars)
				mv.add_var(v);
			for (const auto& v : s2.vars)
				mv.add_var(v);
			S.vars = mv.vars;

			for (const auto& r1 : s1.rows)
			{
				for (const auto& r2 : s2.rows)
				{
					try
					{
						for (const auto& v : r1)
							mv.add_term(v);
						for (const auto& v : r2)
							mv.add_term(v);
						S.rows.emplace_back(mv.take_row());
					}
					catch (Break&) {}
				}
			}
		}

		return S;
	}

	template <>
	auto Database::_query<sparqlxx::Select>(const sparqlxx::Select& s) -> sparqlxx::Solutions
	{
		using sparqlxx::Var;
		using sparqlxx::Term;

		auto op = s.op;
		if (!op || op->is<sparqlxx::Algebra::Null>())
			return {};

		if (op->is<sparqlxx::Algebra::Basic>())
		{
			auto quadpatterns = sparqlxx::QuadsVP{};
			for (auto& p : op->get<sparqlxx::Algebra::Basic>().triples)
				quadpatterns.emplace_back(p.subject, p.predicate, p.object, sparqlxx::Iri{RIRI::DefaultGraphNode});
			op = sparqlxx::Algebra::make<sparqlxx::Algebra::Quad>(std::move(quadpatterns));
		}

		if (op->is<sparqlxx::Algebra::Quad>())
		{
			auto patterns = op->get<sparqlxx::Algebra::Quad>().quads;
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
				}

				solutions.emplace_back(std::move(S));
			}
			return join(std::move(solutions));
		}

		assert(0);
		return {};
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
