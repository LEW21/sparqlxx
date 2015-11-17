#include "tokenize.h"

#include <regex>

class Tokenizer
{
	std::string buffer;

public:
	std::vector<std::string> tokens;
	void tokenize_data(const std::string& data);
	void end();
};

auto spaces = std::string(" \f\n\r\t\v");
auto special = std::string{"<>'\"?$,;.()[]{}@"};

bool is_space[256];
bool is_special[256];

void init_is_space_special()
{
	for (auto i = 0; i < 256; ++i)
	{
		is_space[i] = false;
		is_special[i] = false;
	}

	for (auto c : spaces)
		is_space[c] = true;

	for (auto c : special)
		is_special[c] = true;
}

template <typename F>
void findT(const std::string& buffer, int& offset, F predicate)
{
	bool backslashed = false;
	bool found = false;

	for (; offset < buffer.size(); ++offset)
	{
		if (backslashed)
		{
			backslashed = false;
			continue;
		}

		char c = buffer[offset];
		if (predicate(c))
		{
			found = true;
			break;
		}
		else if (c == '\\')
		{
			backslashed = true;
		}
	}

	if (!found)
		offset = -1;
}

void find(const char& wanted, const std::string& buffer, int& offset)
{
	findT(buffer, offset, [=](char c){return c == wanted;});

	if (offset != -1)
		++offset;
}

void findBoundary(const std::string& buffer, int& offset)
{
	findT(buffer, offset, [=](char c){return is_space[c] || is_special[c];});
}

void findEndl(const std::string& buffer, int& offset)
{
	findT(buffer, offset, [=](char c){return c == '\n' || c == '\r';});

	if (offset != -1)
		++offset;
}

const auto sign     = std::string{"[+-]?"};
const auto exponent = "[eE]" + sign + "[0-9]+";

const auto integer  = sign + "[0-9]+";
const auto decimal  = sign + "([0-9]+\\.[0-9]*|\\.[0-9]+)";
const auto floating = sign + "([0-9]+(\\.[0-9]*)?" + exponent + "|\\.[0-9]+" + exponent + ")";

void Tokenizer::tokenize_data(const std::string& data)
{
	buffer += data;

	while (buffer.size())
	{
		auto first = buffer[0];
		auto offset = 1;

		if (is_space[first])
		{
			// TODO faster
			buffer = buffer.substr(1);
			continue;
		}

		switch (first)
		{
			case '#':
				findEndl(buffer, offset);

				if (offset == -1)
					return;

				buffer = buffer.substr(offset);
				continue;
				break;

			case '<':
				find('>', buffer, offset);
				break;

			case '\'':
			case '"':
			{
				if (buffer[offset] == first && buffer[offset+1] == first)
				{
					// Long string.

					offset += 2;

					do
					{
						find(first, buffer, offset);

						if (offset == -1)
							return;
					}
					while (!(buffer[offset] == first && buffer[offset+1] == first));

					offset += 2;
				}
				else
				{
					find(first, buffer, offset);

					if (offset == -1)
						return;
				}
			}
			break;

			case '?':
			case '$':
			case '@':
				findBoundary(buffer, offset);
				break;

			case '^':
				if (buffer[offset] == '^')
					++offset;
				break;

			case '.':
			case ';':
			case ',':
			case '(':
			case ')':
			case '[':
			case ']':
			case '{':
			case '}':
				break;

			default:
				findBoundary(buffer, offset);

				if (offset == -1)
					return;

				if (buffer[offset] == '.' && std::regex_match(buffer.substr(0, offset), std::regex(::integer)))
				{
					++offset;
					findBoundary(buffer, offset);
				}
				break;
		}

		if (offset == -1)
			return;

		tokens.emplace_back(buffer.substr(0, offset));
		buffer = buffer.substr(offset);
	}
}

void Tokenizer::end()
{
	if (buffer.size())
	{
		tokens.emplace_back(buffer);
		buffer = {};
	}
}

namespace sparqlxx
{
	auto tokenize(const std::string& s) -> std::vector<std::string>
	{
		init_is_space_special();

		Tokenizer t;
		t.tokenize_data(s);
		t.end();
		return t.tokens;
	}
}
