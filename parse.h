#pragma once

#include <string>
#include <vector>
#include "sparql.h"
#include "triples.h"
#include "parse_error.h"

namespace sparqlxx
{
	[[gnu::visibility("default")]]
	/* Parse a SPARQL query
	 * @sparql SPARQL string
	 *
	 * @return corresponding SPARQL command object
	 */
	auto parse(const string& sparql) -> SPARQL;

	[[gnu::visibility("default")]]
	/* Parse a SPARQL query
	 * @tokens SPARQL tokens
	 *
	 * @return corresponding SPARQL command object
	 */
	auto parse(const vector<string>& tokens) -> SPARQL;

	[[gnu::visibility("default")]]
	/* Parse a single triple
	 * @spo string containing subject, predicate and object
	 *
	 * @return <Triple> object
	 */
	auto parse_triple(const string& spo) -> Triple;

	[[gnu::visibility("default")]]
	/* Parse a single quad
	 * @spog string containing subject, predicate, object and a graph
	 *
	 * @return <Quad> object
	 */
	auto parse_quad(const string& spog) -> Quad;
}
