#include "../database.h"
#include <iostream>

using namespace std;
using namespace sparqlxx;

void dump(Triples& tt)
{
	for (const auto& t : tt)
	{
		cout << to_sparql_stmt(t) << endl;
	}
	cout << endl;
}

void dump(const sparqlxx::Solution& s)
{
	if (!s.vars.size())
	{
		std::cout << "No columns." << std::endl;
		return;
	}

	for (auto v : s.vars)
		cout << to_sparql(v) << " ";
	cout << endl;

	for (auto r : s.rows)
	{
		for (auto v : r)
			cout << to_sparql(v) << " ";
		cout << endl;
	}
}

void dump(Database& db)
{
	auto r = db.query(Describe{});
	dump(db.query(Describe{}).get<Triples>());
}

void test_sparqlite(char* dsn)
{
	auto db = Database{dsn};

	db.insert({
		{Iri{"http://lew21.net/a"}, Iri{"b"}, Iri{"c"}, Iri{"d"}}
	});

	dump(db);

	dump(db.select(QuadsVP{
		{Iri{"http://lew21.net/a"}, Iri{"b"}, Var{"o"}, Var{"g"}}
	}));

	db.remove({
		{Iri{"http://lew21.net/a"}, Iri{"b"}, Iri{"c"}, Iri{"d"}}
	});

	dump(db);

	db.insert({
		{Iri{"http://lew21.net/"}, Iri{"name"}, Literal{"Janusz"}, Iri{"urn:x-arq:DefaultGraphNode"}},
		{Iri{"http://lew21.net/"}, Iri{"surname"}, Literal{"Lewandowski"}, Iri{"urn:x-arq:DefaultGraphNode"}},
	});

	dump(db);

	dump(db.select(QuadsVP{
		{Var{"x"}, Iri{"name"}, Literal("Janusz"), Iri{"urn:x-arq:DefaultGraphNode"}},
		{Var{"x"}, Iri{"surname"}, Var("sn"), Iri{"urn:x-arq:DefaultGraphNode"}},
	}));
}

void test_full(char* dsn)
{
	auto db = Database{dsn};

	db.query(R"(
	PREFIX owl: <http://www.w3.org/2002/07/owl#>
	INSERT DATA {
		<isIn> a owl:TransitiveProperty.
		<Warsaw> <isIn> <Poland>.
		<Poland> <isIn> <Europe>.
	})");

	dump(db.query("SELECT * WHERE { <Warsaw> <isIn> ?what }").get<Solution>());
	dump(db.query("SELECT * { ?s ?p ?o }").get<Solution>());
}

int main(int argc, char** argv)
{
	assert(argc >= 2);
	test_sparqlite(argv[1]);
	test_full(argv[1]);
	return 0;
}
