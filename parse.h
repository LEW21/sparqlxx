#pragma once

#include <string>
#include <vector>
#include "sparql.h"
#include "triples.h"
#include "parse_error.h"

namespace sparqlxx
{
	[[gnu::visibility("default")]] auto parse(const string& sparql) -> SPARQL;
	[[gnu::visibility("default")]] auto parse(const vector<string>& tokens) -> SPARQL;

	[[gnu::visibility("default")]] auto parse_triple(const string& spo) -> Triple;
	[[gnu::visibility("default")]] auto parse_quad(const string& spog) -> Quad;
}
