#pragma once

#include "read_triples.h"
#include "token_stream.h"

namespace sparqlxx
{
	template<>
	inline auto token_stream::read<QuadsV>() -> QuadsV
	{
		QuadsV quads;
		// TODO
		parseTriples<TripleV>(*this, [&](TripleV triple){quads.emplace_back(triple.subject, triple.predicate, triple.object, Iri("urn:x-arq:DefaultGraphNode"));});
		return quads;
	}

	template<>
	inline auto token_stream::read<Quads>() -> Quads
	{
		Quads quads;
		auto def_graph = Iri("urn:x-arq:DefaultGraphNode");

		auto stop = true;
		do
		{
			stop = true;

			while (try_match("GRAPH"))
			{
				auto graph = read<Iri>();
				match("{");
				parseTriples<Triple>(*this, [&](Triple triple){quads.emplace_back(triple.subject, triple.predicate, triple.object, graph);});
				match("}");
				stop = false;
			}

			while (parseTriples<Triple>(*this, [&](Triple triple){quads.emplace_back(triple.subject, triple.predicate, triple.object, def_graph);}))
			{
				stop = false;
			}
		} while (!stop);

		return quads;
	}
}
