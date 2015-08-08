#pragma once

#include "atoms.h"
#include "variantxx/variant.hpp"

namespace sparqlxx
{
	using Term = xx::variant<Iri, Literal, BlankNode>;
	using VarOrIri = xx::variant<Var, Iri>;
	using VarOrTerm = xx::variant<Var, Iri, Literal, BlankNode>;
	using PrimaryExpression = xx::variant<Var, Iri, Literal>;

	// TODO
	using Expression = PrimaryExpression;

	// TODO maybe
	using Constraint = PrimaryExpression;

	inline auto to_sparql(const Term& t) -> std::string
	{
		return t.call([](const auto& t){return to_sparql(t);});
	}

	inline auto to_sparql(const VarOrTerm& vt) -> std::string
	{
		return vt.call([](const auto& vt){return to_sparql(vt);});
	}
}
