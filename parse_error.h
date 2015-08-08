#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace sparqlxx
{
	using std::vector;
	using std::string;

	inline auto format_parse_error(vector<string> expected, string got)
	{
		auto error = string{"Expected: "};

		bool first = true;
		for (const auto& e : expected)
		{
			if (!first)
				error += ", ";
			else
				first = false;

			error += e;
		}

		error += "\nGot: " + got;

		return error;
	}

	struct [[gnu::visibility("default")]] parse_error: public std::invalid_argument
	{
		explicit parse_error(vector<string>&& e, const string& g): invalid_argument(format_parse_error(e, g)), expected(std::move(e)), got(g) {}

		vector<string> expected;
		string got;
	};
}
