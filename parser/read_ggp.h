#pragma once

#include "ggp.h"
#include "token_stream.h"

namespace sparqlxx
{
	template<>
	auto token_stream::read<GroupGraphPattern>() -> GroupGraphPattern;
}
