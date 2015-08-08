#pragma once

#include "../dataset.h"
#include "read_atoms.h"

namespace sparqlxx
{
	inline auto read_dataset(token_stream& t, const char* keyword) -> Dataset
	{
		Dataset ds;
		while (t.try_match(keyword))
		{
			if (t.try_match("NAMED"))
				ds.named_graphs.emplace_back(t.read<Iri>());
			else
				ds.default_graph.emplace_back(t.read<Iri>());
		}
		return ds;
	}
}
