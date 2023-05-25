#include "read_ggp.h"

#include "read_query.h"
#include "to_algebra.h"

namespace sparqlxx
{
using namespace Algebra;

inline auto parseGroupGraphPatternSub(token_stream& t) -> AnyOp;
inline auto parseGraphPatternNotTriples(token_stream& t, AnyOp&& gp) -> AnyOp;
inline auto collectTriples(TriplesBlockN& triples, AnyOp&& gp) -> AnyOp;

// SPARQL GroupGraphPattern
template<>
auto token_stream::read<GroupGraphPattern>() -> Algebra::AnyOp
{
	auto ret = Algebra::AnyOp{};
	match("{");
	if (is("SELECT"))
	{
		// SPARQL SubSelect
		auto q = read<Select>();
		if (q.dataset)
			throw std::invalid_argument("SubSelects cannot specify FROM datasets.");
		ret = std::move(q.op);
	}
	else
	{
		ret = parseGroupGraphPatternSub(*this);
	}
	match("}");
	return ret;
}

// SPARQL GroupGraphPatternSub
inline auto parseGroupGraphPatternSub(token_stream& t) -> AnyOp
{
	auto filters = vector<Constraint>{};

	auto triples = t.read<TriplesBlockN>();
	auto gp = make<Null>();

	while (t.is("FILTER") || t.is("{") || t.is("OPTIONAL") || t.is("MINUS") || t.is("GRAPH") || t.is("SERVICE") || t.is("BIND") || t.is("VALUES"))
	{
		if (t.try_match("FILTER"))
		{
			filters.emplace_back(t.read<Constraint>());
		}
		else
		{
			gp = collectTriples(triples, std::move(gp));
			gp = parseGraphPatternNotTriples(t, std::move(gp));
		}
		(void) t.try_match(".");

		auto add_triples = t.read<TriplesBlockN>();
		triples.insert(triples.end(), add_triples.begin(), add_triples.end());
	}

	gp = collectTriples(triples, std::move(gp));

	if (filters.size())
		return make<Filter>(std::move(filters), std::move(gp));
	else
		return gp;
}

inline auto collectTriples(TriplesBlockN& triples, AnyOp&& gp) -> AnyOp
{
	// Collect all triples to a BGP.
	if (triples.size())
	{
		auto t_copy = std::move(triples);
		triples = {};
		if (!gp->is<Null>())
			return make<Join>(std::move(gp), make<Basic>(std::move(t_copy)));
		else
			return make<Basic>(std::move(t_copy));
	}

	return gp;
}

inline auto parseGraphPatternNotTriples(token_stream& t, AnyOp&& gp) -> AnyOp
{
	if (t.is("{"))
	{
		auto tmp = t.read<GroupGraphPattern>();
		while (t.try_match("UNION"))
			tmp = make<Union>(std::move(tmp), t.read<GroupGraphPattern>());
		return make<Join>(std::move(gp), std::move(tmp));
	}

	if (t.try_match("OPTIONAL"))
		return make<LeftJoin>(std::move(gp), t.read<GroupGraphPattern>());

	if (t.try_match("MINUS"))
		return make<Minus>(std::move(gp), t.read<GroupGraphPattern>());

	if (t.try_match("GRAPH"))
	{
		auto graph = t.read<VarOrIri>();
		return make<Join>(std::move(gp), make<Graph>(std::move(graph), t.read<GroupGraphPattern>()));
	}

	if (t.try_match("SERVICE"))
	{
		auto silent = t.try_match("SILENT");
		auto iri = t.read<VarOrIri>();
		return make<Join>(std::move(gp), make<Service>(silent, iri, t.read<GroupGraphPattern>()));
	}

	if (t.try_match("FILTER"))
		throw std::logic_error("FILTER is handled in parseGroupGraphPatternSub.");

	if (t.try_match("BIND"))
	{
		t.match("(");
		auto exp = t.read<Expression>();
		t.match("AS");
		auto var = t.read<Var>();
		t.match(")");
		return make<Extend>(std::move(gp), var, exp);
	}

	//	if (t.try_match("VALUES"))
	//		return Join(std::move(gp), parseDataBlock(t));

	throw std::logic_error{"programming error"};
}
}
