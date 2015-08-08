#include "../parse.h"
#include "../to_sse.h"

using std::vector;
using std::string;

void test_parse_modify()
{
	auto s = sparqlxx::parse({"BASE", "<http://example.org/>", "PREFIX", "x:", "<http://example.com/>", "DELETE", "{", "?s", "?p", "?o", "}", "INSERT", "{", "?s", "?p", "?o", "}", "WHERE", "{", "?o", "?p", "?s", "}"});
}

void test_parse_query()
{
	auto s = sparqlxx::parse(R"(
PREFIX foaf: <http://xmlns.com/foaf/0.1/>
PREFIX dc10: <http://purl.org/dc/elements/1.0/>
PREFIX dc11: <http://purl.org/dc/elements/1.1/>
SELECT ?name ?mbox ?book
WHERE {
	?x foaf:name ?name .
	OPTIONAL {?x foaf:mbox ?mbox}
	OPTIONAL {?x foaf:homepage ?hpage}
	{ ?book dc10:title ?title } UNION { ?book dc11:title ?title }
}
)");

	assert(s.is<sparqlxx::Select>());
	auto sse = sparqlxx::Algebra::to_sse(s.get<sparqlxx::Select>().op);

	auto correct_sse = R"(
(project (?name ?mbox ?book)
	(join
		(leftjoin
			(leftjoin
				(bgp [?x <http://xmlns.com/foaf/0.1/name> ?name])
				(bgp [?x <http://xmlns.com/foaf/0.1/mbox> ?mbox])
			)
			(bgp [?x <http://xmlns.com/foaf/0.1/homepage> ?hpage])
		)
		(union
			(bgp [?book <http://purl.org/dc/elements/1.0/title> ?title])
			(bgp [?book <http://purl.org/dc/elements/1.1/title> ?title])
		)
	)
)
)" + 1;  // +1 - skip first /n

	assert(sse == correct_sse);
}

void test_errors()
{
	try
	{
		sparqlxx::parse("LOLWUT");
	}
	catch (sparqlxx::parse_error& e)
	{
		if (e.expected != vector<string>{"BASE", "PREFIX", "SELECT", "CONSTRUCT", "DESCRIBE", "ASK", "CREATE", "LOAD", "CLEAR", "DROP", "ADD", "MOVE", "COPY", "WITH", "DELETE", "INSERT"})
			throw;
		assert(e.got == "LOLWUT");
	}

	try
	{
		sparqlxx::parse("SELECT LOLWUT");
	}
	catch (sparqlxx::parse_error& e)
	{
		if (e.expected != vector<string>{"DISTINCT", "REDUCED", "*", "Var", "("})
			throw;
		assert(e.got == "LOLWUT");
	}

	try
	{
		sparqlxx::parse("SELECT * LOLWUT");
	}
	catch (sparqlxx::parse_error& e)
	{
		if (e.expected != vector<string>{"FROM", "WHERE", "{"})
			throw;
		assert(e.got == "LOLWUT");
	}

	try
	{
		sparqlxx::parse("SELECT * { LOLWUT");
	}
	catch (sparqlxx::parse_error& e)
	{
		if (e.expected != vector<string>{"SELECT", "(", "[", "Var", "Iri", "BlankNode", "Literal", "FILTER", "{", "OPTIONAL", "MINUS", "GRAPH", "SERVICE", "BIND", "VALUES", "}"})
			throw;
		assert(e.got == "LOLWUT");
	}

	try
	{
		sparqlxx::parse("DELETE {?s ?p ?o}");
	}
	catch (sparqlxx::parse_error& e)
	{
		if (e.expected != vector<string>{"INSERT", "USING", "WHERE"})
			throw;
		assert(e.got == "end-of-input");
	}
}

int main()
{
	test_parse_modify();
	test_parse_query();
	test_errors();
	return 0;
}
