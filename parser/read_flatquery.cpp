#include "read_query.h"

#include "read_atoms.h"
#include "read_dataset.h"
#include "read_ggp.h"
#include "read_unions.h"
#include "read_triples.h"

namespace sparqlxx
{
inline auto parseWhereClause(token_stream& t) -> GroupGraphPattern;
inline auto parseSolutionModifier(token_stream& t, FlatQueryOp&) -> void;
//inline auto parseValuesClause(token_stream& t);

inline auto parseWhereClause(token_stream& t) -> GroupGraphPattern
{
	(void) t.try_match("WHERE");

	return t.read<GroupGraphPattern>();
}

inline auto parseSolutionModifier(token_stream& t, FlatQueryOp& op) -> void
{
	if (t.try_match("GROUP"))
	{
		t.match("BY");

		// TODO
		throw std::invalid_argument("GROUP BY is not yet supported.");
	}

	if (t.try_match("HAVING"))
	{
		do
			op.having.emplace_back(t.read<Constraint>());
		while (t.is<Constraint>());
	}

	if (t.try_match("ORDER"))
	{
		t.match("BY");

		// TODO
		throw std::invalid_argument("ORDER BY is not yet supported.");
	}

	if (t.try_match("LIMIT"))
	{
		op.limit = t.read<int>();

		if (t.try_match("OFFSET"))
			op.offset = t.read<int>();
	}
	else if (t.try_match("OFFSET"))
	{
		op.offset = t.read<int>();

		if (t.try_match("LIMIT"))
			op.limit = t.read<int>();
	}
}

template<>
auto token_stream::read<FlatSelect>() -> FlatSelect
{
	match("SELECT");
	auto q = FlatSelect{};

	if (try_match("DISTINCT"))
		q.op.distinct = true;
	else if (try_match("REDUCED"))
		q.op.reduced = true;

	if (!try_match("*"))
	{
		q.op.projection = vector<Binding>{};

		do
		{
			if (is<Var>())
				q.op.projection->emplace_back(read<Var>());
			else if (try_match("("))
			{
				auto expr = read<Expression>();
				match("AS");
				auto var = read<Var>();
				match(")");
				q.op.projection->emplace_back(std::move(expr), std::move(var));
			}
			else
				throw error();
		}
		while (is<Var>() || is("("));
	}

	q.query.dataset = read_dataset(*this, "FROM");
	q.op.where = parseWhereClause(*this);

	parseSolutionModifier(*this, q.op);
	//q.op.values = parseValuesClause(*this);
	return q;
}

template<>
auto token_stream::read<FlatConstruct>() -> FlatConstruct
{
	match("CONSTRUCT");
	auto q = FlatConstruct{};

	if (is("{"))
	{
		match("{");
		q.query.tpl = read<TriplesTemplateN>();
		match("}");
		q.query.dataset = read_dataset(*this, "FROM");
		q.op.where = parseWhereClause(*this);
	}
	else if (is("FROM") || is("WHERE"))
	{
		q.query.dataset = read_dataset(*this, "FROM");
		match("WHERE");
		match("{");
		q.query.tpl = read<TriplesTemplateN>();
		q.op.where = Algebra::make<Algebra::Basic>(q.query.tpl);
		match("}");
	}
	else
		throw error();

	parseSolutionModifier(*this, q.op);
	//q.op.values = parseValuesClause(*this);
	return q;
}

template<>
auto token_stream::read<FlatDescribe>() -> FlatDescribe
{
	match("DESCRIBE");
	auto q = FlatDescribe{};

	if (!try_match("*"))
	{
		q.op.projection = vector<Binding>{};

		do
		{
			auto vi = read<VarOrIri>();
			if (vi.is<Var>())
				q.op.projection->emplace_back(vi.get<Var>());
			q.query.terms.emplace_back(std::move(vi));
		}
		while (is<VarOrIri>());
	}

	q.query.dataset = read_dataset(*this, "FROM");
	if (is("WHERE") || is("{"))
		q.op.where = parseWhereClause(*this);

	parseSolutionModifier(*this, q.op);
	//q.op.values = parseValuesClause(*this);
	return q;
}

template<>
auto token_stream::read<FlatAsk>() -> FlatAsk
{
	match("ASK");
	auto q = FlatAsk{};

	q.query.dataset = read_dataset(*this, "FROM");
	q.op.where = parseWhereClause(*this);
	parseSolutionModifier(*this, q.op);
	//q.op.values = parseValuesClause(*this);
	return q;
}
}
