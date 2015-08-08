#pragma once

#include <string>
#include <vector>

namespace sparqlxx
{
	auto tokenize(const std::string&) -> std::vector<std::string>;
}
