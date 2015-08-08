#include "read_prologue.h"
#include "read_flatquery.h"
#include "tokenize.h"
#include <cassert>
#include <iostream>
using namespace std;
using sparqlxx::token_stream;
using sparqlxx::Prologue;
using sparqlxx::FlatSelect;

void test_tokenize()
{
	auto tokens = sparqlxx::tokenize("BASE <http://example.org/> PREFIX x: <http://example.com/> SELECT DISTINCT ?p (?o as ?x) WHERE {?s ?p ?o; x:name \"aaa\"@pl; x:desc \"5\"^^x:int } LIMIT 5 OFFSET 20");
	auto tokens_ok = vector<string>{"BASE", "<http://example.org/>", "PREFIX", "x:", "<http://example.com/>", "SELECT", "DISTINCT", "?p", "(", "?o", "as", "?x", ")", "WHERE", "{", "?s", "?p", "?o", ";", "x:name", "\"aaa\"", "@pl", ";", "x:desc", "\"5\"", "^^", "x:int", "}", "LIMIT", "5", "OFFSET", "20"};
	assert(tokens == tokens_ok);
}

void test_parse_flatselect_minimal()
{
	auto tokens = vector<string>{"SELECT", "*", "WHERE", "{", "}"};
	auto t = token_stream{tokens};
	t.read<Prologue>();
	auto q = t.read<FlatSelect>();
	t.match_end();

	assert(q.op.distinct == false);
	assert(q.op.reduced == false);
	assert(!q.op.projection);
	assert(!q.query.dataset);
	assert(q.op.where);
	assert(q.op.where->is<sparqlxx::Algebra::Null>());
	q.op.where->get<sparqlxx::Algebra::Null>();
	assert(!q.op.limit);
	assert(!q.op.offset);
	//assert(!q.op.values);
}

void test_parse_flatselect()
{
	auto tokens = vector<string>{"BASE", "<http://example.org/>", "PREFIX", "x:", "<http://example.com/>", "SELECT", "DISTINCT", "?p", "(", "?o", "as", "?x", ")", "WHERE", "{", "<a>", "?p", "?o", ";", "x:y", "\"lorem\"", "FILTER", "$p", "}", "HAVING", "(", "?o", ")", "LIMIT", "5", "OFFSET", "20"};
	auto t = token_stream{tokens};
	t.read<Prologue>();
	auto q = t.read<FlatSelect>();
	t.match_end();

	assert(q.op.distinct == true);
	assert(q.op.reduced == false);
	assert(q.op.projection);
	assert(q.op.projection->size() == 2);
	assert((*q.op.projection)[0].from.is<sparqlxx::Var>());
	assert((*q.op.projection)[0].from.get<sparqlxx::Var>().name == "p");
	assert((*q.op.projection)[0].to.name == "p");
	assert((*q.op.projection)[1].from.is<sparqlxx::Var>());
	assert((*q.op.projection)[1].from.get<sparqlxx::Var>().name == "o");
	assert((*q.op.projection)[1].to.name == "x");
	assert(!q.query.dataset);
	assert(q.op.where);
	assert(q.op.where->is<sparqlxx::Algebra::Filter>());
	const auto& f = q.op.where->get<sparqlxx::Algebra::Filter>();

	assert(f.constraints.size() == 1);
	assert(f.data);
	assert(f.data->is<sparqlxx::Algebra::Basic>());
	const auto& b = f.data->get<sparqlxx::Algebra::Basic>();
	assert(b.triples.size() == 2);

	assert(b.triples[0].subject.is<sparqlxx::Iri>());
	assert(b.triples[0].subject.get<sparqlxx::Iri>().iri == "http://example.org/a");
	assert(b.triples[0].predicate.is<sparqlxx::Var>());
	assert(b.triples[0].predicate.get<sparqlxx::Var>().name == "p");
	assert(b.triples[0].object.is<sparqlxx::Var>());
	assert(b.triples[0].object.get<sparqlxx::Var>().name == "o");

	assert(b.triples[1].subject.is<sparqlxx::Iri>());
	assert(b.triples[1].subject.get<sparqlxx::Iri>().iri == "http://example.org/a");
	assert(b.triples[1].predicate.is<sparqlxx::Iri>());
	assert(b.triples[1].predicate.get<sparqlxx::Iri>().iri == "http://example.com/y");
	assert(b.triples[1].object.is<sparqlxx::Literal>());
	assert(b.triples[1].object.get<sparqlxx::Literal>().value == "lorem");

	assert(q.op.limit);
	assert(*q.op.limit == 5);
	assert(q.op.offset);
	assert(*q.op.offset == 20);
	//assert(!q.op.values);
}


int main()
{
	test_tokenize();
	test_parse_flatselect_minimal();
	test_parse_flatselect();
	return 0;
}
