#include "../parse.h"

#include "read_prologue.h"
#include "read_query.h"
#include "read_update.h"
#include "tokenize.h"

namespace sparqlxx
{
	auto parse(const vector<string>& tokens) -> SPARQL
	{
		auto t = token_stream{tokens};
		t.read<Prologue>();

		if (t.is("SELECT") || t.is("CONSTRUCT") || t.is("DESCRIBE") || t.is("ASK"))
		{
			auto q = t.read<Query>();
			t.match_end();
			return std::move(q);
		}

		auto u = t.read<Update>();
		t.match_end();
		return std::move(u);
	}

	auto parse(const string& sparql) -> SPARQL
	{
		return parse(tokenize(sparql));
	}

	auto parse_triple(const string& spo) -> Triple
	{
		auto tokens = tokenize(spo);
		auto t = token_stream{tokens};
		auto r = Triple{t.read<Term>(), t.read<Iri>(), t.read<Term>()};
		t.match_end();
		return r;
	}

	auto parse_quad(const string& spog) -> Quad
	{
		auto tokens = tokenize(spog);
		auto t = token_stream{tokens};
		auto r = Quad{t.read<Term>(), t.read<Iri>(), t.read<Term>(), t.read<Iri>()};
		t.match_end();
		return r;
	}
}
