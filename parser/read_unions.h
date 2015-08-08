#pragma once

#include "../unions.h"
#include "token_stream.h"
#include "read_atoms.h"

namespace sparqlxx
{
	template<>
	inline auto token_stream::is<Term>() -> bool
	{
		return is<Iri>() || is<BlankNode>() || is<Literal>();
	}

	template<>
	inline auto token_stream::read<Term>() -> Term
	{
		if (is<Iri>())
			return read<Iri>();
		else if (is<BlankNode>())
			return read<BlankNode>();
		else
			return read<Literal>();
	}

	template<>
	inline auto token_stream::is<VarOrIri>() -> bool
	{
		return is<Var>() || is<Iri>();
	}

	template<>
	inline auto token_stream::read<VarOrIri>() -> VarOrIri
	{
		if (is<Var>())
			return read<Var>();
		else
			return read<Iri>();
	}

	template<>
	inline auto token_stream::is<VarOrTerm>() -> bool
	{
		return is<Var>() || is<Term>();
	}

	template<>
	inline auto token_stream::read<VarOrTerm>() -> VarOrTerm
	{
		if (is<Var>())
			return read<Var>();
		else
			return read<Term>();
	}

	template<>
	inline auto token_stream::is<PrimaryExpression>() -> bool
	{
		// TODO FunctionCall BuiltInCall
		return is("(") || is<Var>() || is<Iri>() || is<Literal>();
	}

	template<>
	inline auto token_stream::read<PrimaryExpression>() -> PrimaryExpression
	{
		// TODO FunctionCall BuiltInCall
		if (try_match("(")) // BrackettedExpression
		{
			auto expr = read<Expression>();
			match(")");
			return expr;
		}
		else if (is<Var>())
			return read<Var>();
		else if (is<Iri>())
			return read<Iri>();
		else
			return read<Literal>();
	}
}
