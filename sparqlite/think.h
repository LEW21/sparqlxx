#include "database.h"

namespace sparqlite
{
	struct InferredTriple
	{
		Triple s;
		std::vector<ID> inferredFrom;

		InferredTriple(Triple s, std::vector<ID> i): s(std::move(s)), inferredFrom(std::move(i)) {}
	};

	auto think(const Database&, ID nsID, Quad newStatement) -> std::vector<InferredTriple>;
}
