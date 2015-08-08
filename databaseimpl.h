#pragma once

#include "sparql.h"
#include "result.h"

namespace sparqlxx
{
	class [[gnu::visibility("default")]] DatabaseImpl
	{
		DatabaseImpl(const DatabaseImpl&) = delete;

	protected:
		DatabaseImpl() {}

	public:
		virtual ~DatabaseImpl() {}

		virtual auto query(const SPARQL&) -> Result = 0;
	};
}
