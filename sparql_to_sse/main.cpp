#include "../parse.h"
#include "../to_sse.h"
#include "../algebratransformers/to_quads.h"
#include "../algebratransformers/merge_joins.h"
#include "../algebratransformers/merge_basics.h"
#include <iostream>

using namespace sparqlxx;
using namespace sparqlxx::Algebra;

int main(int argc, char** argv)
{
	auto optimize = (argc == 2 && std::string(argv[1]) == "-o");
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

	if (optimize)
	{
		using namespace sparqlxx::AlgebraTransformers;
		*algebra = merge_basics(merge_joins(to_quads(std::move(*algebra), sparqlxx::Iri{"http://example.com/default-graph"})));
	}

	std::cout << to_sse(algebra) << std::flush;
	return 0;
}
