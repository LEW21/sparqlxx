#pragma once

#include "../triples.h"
#include "../RDF.h"
#include "token_stream.h"

namespace sparqlxx
{
	/*
	 * GraphTerm         := iri | RDFLiteral | NumericLiteral | BooleanLiteral | BlankNode | NIL
	 * IndirectGraphTerm := GraphTerm | Collection | BlankNodePropertyList
	 * VarOrTerm         := Var | IndirectGraphTerm
	 *
	 * Predicate can also be 'a'.
	 * Predicate can also be a Path, check it with before GraphTerm as 'a' and iri are valid path prefixes.
	 */

	template <typename T, typename F>
	auto parseTriples(token_stream& t, F&& yield) -> bool
	{
		using S = decltype(std::declval<T>().subject);
		using P = decltype(std::declval<T>().predicate);
		using O = decltype(std::declval<T>().object);

		bool yielded = false;

		Subject:
			const bool requirePropertyList = !t.is("(") && !t.is("[");

			if (!t.is<S>())
				return yielded;

			auto subject = t.read<S>();

			if (!requirePropertyList && !t.is<P>())
			{
				if (t.try_match("."))
					goto Subject;
				else
					return yielded;
			}

		Predicate:
			auto predicate = t.try_match("a") ? Iri(RDF::type) : t.read<P>();

		Object:
			auto object = t.read<O>();

			yield(T{subject, predicate, object});
			yielded = true;

			if (t.try_match(","))
				goto Object;
			else if (t.try_match(";"))
				goto Predicate;
			else if (t.try_match("."))
				goto Subject;
			else
				return yielded;
	}

	template<>
	auto token_stream::is<TriplesTemplateN>() -> bool; // undefined as it's hard to determine

	template<>
	inline auto token_stream::read<TriplesTemplateN>() -> TriplesTemplateN
	{
		TriplesTemplateN triples;
		parseTriples<TripleV>(*this, [&](TripleV triple){triples.emplace_back(std::move(triple));});
		return triples;
	}
}
