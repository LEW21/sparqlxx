#include "../sparqlite/database.h"
#include "../parse.h"

#include <boost/archive/tmpdir.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <sstream>

using namespace sparqlite;

void test_serialization()
{
	std::stringstream ss;

	Database a;

	a.query(sparqlxx::parse(R"(
		BASE <tag:xtreeme.org,2020:tests>
		PREFIX owl: <http://www.w3.org/2002/07/owl#>
		INSERT DATA {
			<#Warsaw> <#isIn> <#Poland>.
			<#Poland> <#isIn> <#Europe>.
		})"));

	{
		boost::archive::binary_oarchive oa{ss};
		oa << a;
	}

	Database b;
	{
		boost::archive::binary_iarchive ia{ss};
		ia >> b;
	}

	auto s = b.query(sparqlxx::parse("BASE <tag:xtreeme.org,2020:tests> SELECT * { <#Warsaw> <#isIn> ?y }")).get<sparqlxx::Solutions>();
	assert(s.vars.size() == 1);
	assert(s.vars[0] == sparqlxx::Var{"y"});
	assert(s.rows.size() == 1);
	assert(s.rows[0].size() == 1);
	assert(s.rows[0][0].is<sparqlxx::Iri>());
	assert(s.rows[0][0].get<sparqlxx::Iri>() == sparqlxx::Iri{"tag:xtreeme.org,2020:tests#Poland"});

	s = b.query(sparqlxx::parse("BASE <tag:xtreeme.org,2020:tests> SELECT * { <#Poland> <#isIn> ?y }")).get<sparqlxx::Solutions>();
	assert(s.vars.size() == 1);
	assert(s.vars[0] == sparqlxx::Var{"y"});
	assert(s.rows.size() == 1);
	assert(s.rows[0].size() == 1);
	assert(s.rows[0][0].is<sparqlxx::Iri>());
	assert(s.rows[0][0].get<sparqlxx::Iri>() == sparqlxx::Iri{"tag:xtreeme.org,2020:tests#Europe"});
}

int main()
{
	test_serialization();
	return 0;
}
