#pragma once

#include "parse.h"

namespace sparqlxx
{
	inline bool _autocomplete_iequals(const string& a, const string& b, int count)
	{
		for (size_t i = 0; i < count; ++i)
			if (tolower(a[i]) != tolower(b[i]))
				return false;
			return true;
	}

	auto autocomplete(const std::string& buffer) -> std::vector<std::string>
	{
		try
		{
			parse(buffer + std::string{"\0", 1});
		}
		catch (parse_error& e)
		{
			if (buffer.size() && buffer[buffer.size() - 1] != ' ' && e.got[0] == '\0')
				return {std::string{" "}};

			if (e.got[e.got.size() - 1] == '\0')
			{
				bool can_end = false;
				auto matches = std::vector<std::string>{};
				for (auto ex : e.expected)
				{
					auto got = e.got.substr(0, e.got.size() - 1);
					if (ex == "end")
					{
						if (got.size() == 0)
							can_end = true;
						continue;
					}
					if (ex == "Var")
					{
						ex = "?";
						if (got.size() && (got[0] == '?' || got[0] == '$'))
						{
							if (got.size() > 1)
								return {std::string{" "}};
							else
								return {};
						}
					}
					if (ex == "Iri")
					{
						ex = "<";
						if (got.size() && got[0] == '<')
							return {got + ">"};
					}
					if (ex == "BlankNode")
					{
						ex = "_:";
						if (got.size() >= 2 && got[0] == '_' && got[1] == ':')
						{
							if (got.size() >= 3)
								return {std::string{" "}};
							else
								return {};
						}
					}
					if (ex == "Literal")
					{
						ex = "\"";
						if (got.size() && (got[0] == '"' || got[0] == '\''))
							return {got + got[0]};
					}
					if (ex == "int")
					{
						if (got.size())
						{
							try
							{
								std::stoi(got);
								return {got};
							}
							catch (std::invalid_argument&) {}
						}
						continue;
					}
					if (_autocomplete_iequals(ex, e.got, e.got.size() - 1))
						matches.emplace_back(ex);
				}

				if (matches.size() || can_end)
					return matches;
			}

			throw;
		}

		return {};
	}
}
