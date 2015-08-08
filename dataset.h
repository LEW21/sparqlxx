#pragma once

#include "atoms.h"

namespace sparqlxx
{
	struct [[gnu::visibility("default")]] Dataset
	{
		vector<Iri> default_graph;
		vector<Iri> named_graphs;

		inline operator bool() const { return default_graph.size() || named_graphs.size(); }
	};
}
