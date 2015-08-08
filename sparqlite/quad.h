#pragma once

#include "../triples.h"
#include "id.h"

namespace sparqlite
{
	using Triple = sparqlxx::TripleT<ID, ID, ID>;
	using Quad = sparqlxx::QuadT<ID, ID, ID, ID>;
}

/*
namespace sparqlxx
{
	inline auto operator==(const sparqlite::Quad& a, const sparqlite::Quad& b) -> bool
	{
		return a.subject == b.subject && a.predicate == b.predicate && a.object == b.object && a.graph == b.graph;
	}
	inline auto operator!=(const sparqlite::Quad& a, const sparqlite::Quad& b) -> bool { return !(a == b); }
}

namespace std
{
	template<>
	struct hash<sparqlite::Quad>
	{
		typedef sparqlite::Quad argument_type;
		typedef std::size_t result_type;

		inline result_type operator()(const sparqlite::Quad& q) const
		{
			return q.subject ^ q.predicate ^ q.object ^ q.graph;
		}
	};
}
*/
