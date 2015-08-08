#pragma once

#include "read_atoms.h"

namespace sparqlxx
{
	struct Prologue {};

	template<>
	auto token_stream::is<Prologue>() -> bool; // undefined as it's hard to determine

	template<>
	inline auto token_stream::read<Prologue>() -> Prologue
	{
		while (true)
		{
			if (try_match("BASE"))
			{
				base = read<Iri>();
				continue;
			}

			if (try_match("PREFIX"))
			{
				auto prefix = read<Prefix>();
				prefixes[prefix.name] = read<Iri>();
				continue;
			}

			break;
		}

		return {};
	}
}
