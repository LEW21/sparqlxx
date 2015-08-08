#pragma once

#include "query.h"
#include "update.h"

namespace sparqlxx
{
	using SPARQL = xx::variant<Select, Construct, Describe, Ask, InsertData, DeleteData, Modify, MassOp, Create, Load, Clear, Drop>;
}
