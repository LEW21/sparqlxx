#include "../database.h"
#include "../RDF.h"

using namespace sparqlxx;

void test_insert_select_match(char* dsn)
{
	auto db = Database{dsn};

	db.insert({{Iri{"http://lew21.net/a"}, Iri{RDF::type}, Iri{"http://lew21.net/exampletype"}, Iri{"http://whatever"}}});

	auto res = db.select({{Var{"obj"}, Iri{RDF::type}, Iri{"http://lew21.net/exampletype"}, Iri{"http://whatever"}}});
	assert(res.vars.size() == 1);
	assert(res.vars[0] == Var{"obj"});
	assert(res.rows.size() == 1);
	assert(res.rows[0].size() == 1);
	assert(res.rows[0][0].is<Iri>());
	assert(res.rows[0][0].get<Iri>() == Iri{"http://lew21.net/a"});
}

void test_insert_select_nomatch(char* dsn)
{
	auto db = Database{dsn};

	db.insert({{Iri{"http://lew21.net/a"}, Iri{RDF::type}, Iri{"http://lew21.net/exampletype"}, Iri{"http://whatever"}}});

	auto res = db.select({{Var{"obj"}, Iri{RDF::type}, Iri{"XXX"}, Iri{"http://whatever"}}});
	assert(res.vars.size() == 1);
	assert(res.vars[0] == Var{"obj"});
	assert(res.rows.size() == 0);
}

void test_insert_select_two_patterns(char* dsn)
{
	auto db = Database{dsn};

	db.insert({
		{Iri{"http://lew21.net/a"}, Iri{RDF::type}, Iri{"http://lew21.net/exampletype"}, Iri{"http://whatever"}},
		{Iri{"http://lew21.net/exampletype"}, Iri{RDF::type}, Iri{RDF::Class}, Iri{"http://whatever"}},
	});

	auto res = db.select({
		{Var{"obj"}, Iri{RDF::type}, Var{"class"}, Iri{"http://whatever"}},
		{Var{"class"}, Iri{RDF::type}, Iri{RDF::Class}, Iri{"http://whatever"}},
	});

	assert(res.vars.size() == 2);
	assert(res.vars[0] == Var{"obj"});
	assert(res.vars[1] == Var{"class"});
	assert(res.rows.size() == 1);
	assert(res.rows[0].size() == 2);
	assert(res.rows[0][0].is<Iri>());
	assert(res.rows[0][0].get<Iri>() == Iri{"http://lew21.net/a"});
	assert(res.rows[0][1].is<Iri>());
	assert(res.rows[0][1].get<Iri>() == Iri{"http://lew21.net/exampletype"});
}

void test_ask(char* dsn)
{
	auto db = Database{dsn};

	db.query(R"(
		PREFIX owl: <http://www.w3.org/2002/07/owl#>
		INSERT DATA {
			<isIn> a owl:TransitiveProperty.
			<Warsaw> <isIn> <Poland>.
			<Poland> <isIn> <Europe>.
		})");

	auto res = db.query("ASK { <Warsaw> <isIn> <Poland>. }");
	assert(res.is<bool>());
	assert(res.get<bool>() == true);

	res = db.query("ASK { <Warsaw> <isIn> <Asia>. }");
	assert(res.is<bool>());
	assert(res.get<bool>() == false);
}

void test_ask_reasoner(char* dsn)
{
	auto db = Database{dsn};

	db.query(R"(
		PREFIX owl: <http://www.w3.org/2002/07/owl#>
		INSERT DATA {
			<isIn> a owl:TransitiveProperty.
			<Warsaw> <isIn> <Poland>.
			<Poland> <isIn> <Europe>.
		})");

	auto res = db.query("ASK { <Warsaw> <isIn> <Europe>. }");
	assert(res.is<bool>());
	assert(res.get<bool>() == true);
}

void test_insert_delete(char* dsn)
{
	auto db = Database{dsn};

	auto res = db.query("ASK { <Warsaw> <isIn> <Poland>. }");
	assert(res.is<bool>() && res.get<bool>() == false);

	db.query(R"(
		INSERT DATA {
			<Warsaw> <isIn> <Poland>.
		})");

	res = db.query("ASK { <Warsaw> <isIn> <Poland>. }");
	assert(res.is<bool>() && res.get<bool>() == true);

	db.query(R"(
		DELETE DATA {
			<Warsaw> <isIn> <Poland>.
		})");

	res = db.query("ASK { <Warsaw> <isIn> <Poland>. }");
	assert(res.is<bool>() && res.get<bool>() == false);
}

int main(int argc, char** argv)
{
	assert(argc >= 2);
	test_insert_select_match(argv[1]);
	test_insert_select_nomatch(argv[1]);
	test_insert_select_two_patterns(argv[1]);
	test_ask(argv[1]);
	test_ask_reasoner(argv[1]);
	test_insert_delete(argv[1]);
	return 0;
}
