#include <string>

namespace sparqlxx
{
	template <char begin, char end = begin>
	inline auto quote(const std::string& v)
	{
		auto turtle = std::string{};
		turtle.reserve(v.size() * 2);

		turtle += begin;

		for (auto c : v)
			switch (c)
			{
				case '\\':
					turtle += "\\\\";
					break;

				case '\t':
					turtle += "\\t";
					break;

				case '\n':
					turtle += "\\n";
					break;

				case '\r':
					turtle += "\\r";
					break;

				case end:
					turtle += "\\";
					turtle += end;
					break;

				default:
					turtle += c;
					break;
			}

		turtle += end;
		return turtle;
	}
}
