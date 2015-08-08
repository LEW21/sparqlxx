#pragma once

#include "unions.h"

namespace sparqlxx
{
	template <typename S, typename P, typename O>
	struct [[gnu::visibility("default")]] TripleT
	{
		S subject; // TODO TriplesNode
		P predicate;
		O object; // TODO TriplesNode
	};

	template <typename S, typename P, typename O, typename G>
	struct [[gnu::visibility("default")]] QuadT
	{
		S subject; // TODO TriplesNode
		P predicate;
		O object; // TODO TriplesNode
		G graph;

		QuadT(S s, P p, O o, G g): subject(std::move(s)), predicate(std::move(p)), object(std::move(o)), graph(std::move(g)) {}
	};

	using Triple = TripleT<Term, Iri, Term>;
	using Triples = vector<Triple>;

	using TripleV = TripleT<VarOrTerm, VarOrIri, VarOrTerm>;
	using TriplesV = vector<TripleV>;

	// TODO: Add Path to predicates
	using TripleVP = TripleV;
	using TriplesVP = vector<TripleVP>;

	using Quad = QuadT<Term, Iri, Term, Iri>;
	using Quads = vector<Quad>;

	using QuadV = QuadT<VarOrTerm, VarOrIri, VarOrTerm, VarOrIri>;
	using QuadsV = vector<QuadV>;

	// TODO: Add Path to predicates
	using QuadVP = QuadV;
	using QuadsVP = vector<QuadVP>;

	// SPARQL productions
	using TriplesTemplateN = TriplesV;
	using TriplesBlockN = TriplesVP;
	using QuadDataN = Quads;
	using QuadPatternN = QuadsV;

	template <typename S, typename P, typename O>
	inline auto to_sparql_stmt(const TripleT<S, P, O>& t) -> std::string
	{
		return to_sparql(t.subject) + " " + to_sparql(t.predicate) + " " + to_sparql(t.object) + ".";
	}

	template <typename S, typename P, typename O, typename G>
	inline auto to_sparql_stmt(const QuadT<S, P, O, G>& t) -> std::string
	{
		return "GRAPH " + to_sparql(t.graph) + " {" + to_sparql(t.subject) + " " + to_sparql(t.predicate) + " " + to_sparql(t.object) + ".}";
	}

}
