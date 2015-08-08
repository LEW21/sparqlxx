#include "../parse.h"
#include "../to_sse.h"
#include <iostream>

using namespace sparqlxx;
using namespace sparqlxx::Algebra;

int main()
{
	auto sparql = std::string{std::istreambuf_iterator<char>{std::cin}, std::istreambuf_iterator<char>{}};
	auto q = parse(sparql);
	auto algebra =
		(q.is<Select>())    ? q.get<Select>().op :
		(q.is<Construct>()) ? q.get<Construct>().op :
		(q.is<Describe>())  ? q.get<Describe>().op :
		(q.is<Ask>())       ? q.get<Ask>().op :
		(q.is<Modify>())    ? q.get<Modify>().where :
		AnyOp{};

	if (!algebra)
	{
		std::cout << "Your query does not use SPARQL algebra." << std::endl;
		return -1;
	}

	std::cout << to_sse(algebra) << std::flush;
	return 0;
}
