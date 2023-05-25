#include "url.h"

#include <optional>
#include <variant>
#include <vector>
#include <stdexcept>
#include <codecvt>
#include <locale>

namespace sparqlxx
{
using PES = auto(*)(char32_t) -> bool;

const auto hex_digits = "0123456789ABCDEF";

inline auto utf8_percent_encode(char32_t input, const PES& pes) -> std::string
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8conv;

	auto encoded_input = utf8conv.to_bytes(std::u32string{input});

	auto newlen = 0;
	for (auto c : encoded_input) newlen += pes(c) ? 3 : 1;
	auto buffer = std::string{};
	buffer.reserve(newlen);

	for (auto c : encoded_input)
	{
		if (pes(c))
		{
			buffer += '%';
			buffer += hex_digits[uint8_t(c) / 16];
			buffer += hex_digits[uint8_t(c) % 16];
		}
		else
		{
			buffer += c;
		}
	}
	return buffer;
}

inline auto pes_c0_control(char32_t input) -> bool
{
	return input < 0x1f || input > 0x7e;
}

inline auto pes_fragment(char32_t input) -> bool
{
	if (pes_c0_control(input)) return true;

	switch (input)
	{
		case ' ':
		case '"':
		case '<':
		case '>':
		case '`':
			return true;
	}
	return false;
}

inline auto pes_query(char32_t input) -> bool
{
	if (pes_c0_control(input)) return true;

	switch (input)
	{
		case ' ':
		case '"':
		case '<':
		case '>':
		case '#':
			return true;
	}
	return false;
}

inline auto pes_path(char32_t input) -> bool
{
	if (pes_query(input)) return true;

	switch (input)
	{
		case '?':
		case '`':
		case '{':
		case '}':
			return true;
	}
	return false;
}

inline auto pes_userinfo(char32_t input) -> bool
{
	if (pes_path(input)) return true;

	switch (input)
	{
		case '/':
		case ':':
		case ';':
		case '=':
		case '@':
		case '[':
		case '\\':
		case ']':
		case '^':
		case '|':
			return true;
	}
	return false;
}

auto shorten_path(const URL::Path& path) -> URL::Path
{
	if (std::holds_alternative<std::string>(path))
		std::terminate();

	auto new_path = std::get<std::vector<std::string>>(path);
	if (new_path.size())
		new_path.pop_back();
	return new_path;
}

auto parse_host(const std::u32string& input) -> std::string
{
	if (input[0] == '[')
	{
		if (input[input.size() - 1] != ']')
		{
			throw std::invalid_argument{"IPv6-unclosed"};
		}
	}
	auto buffer = std::string{};
	for (auto c : input)
	{
		buffer += utf8_percent_encode(c, pes_c0_control);
	}
	return buffer;
}

enum class URLParserState {
	SchemeStartState,
	SchemeState,
	NoSchemeState,
	PathOrAuthorityState,
	RelativeState,
	RelativeSlashState,
	AuthorityState,
	HostState,
	PortState,
	PathStartState,
	PathState,
	OpaquePathState,
	QueryState,
	FragmentState,
};

auto parse_url(const std::u32string& input, const std::optional<URL>& base, URL& url) -> void
{
	auto schemeBuffer = std::string{};
	auto authorityBuffer = std::u32string{};
	auto hostBuffer = std::u32string{};
	auto portBuffer = std::string{};
	auto pathSegmentBuffer = std::string{};

	auto state = URLParserState::SchemeStartState;
	auto atSignSeen = false;
	auto insideBrackets = false;
	auto passwordTokenSeen = false;

	for (auto pointer = 0; pointer <= input.size(); ++pointer)
	{
		const auto c = input[pointer];
		switch (state)
		{
			case URLParserState::SchemeStartState:
				if (c >= 'a' && c <= 'z')
				{
					schemeBuffer += c;
					state = URLParserState::SchemeState;
				}
				else if (c >= 'A' && c <= 'Z')
				{
					schemeBuffer += c - 'A' + 'a';
					state = URLParserState::SchemeState;
				}
				else
				{
					state = URLParserState::NoSchemeState;
					pointer -= 1;
				}
			break;

			case URLParserState::SchemeState:
				if ((c >= 'a' && c <= 'z') || c == '+' || c == '-' || c == '.')
				{
					schemeBuffer += c;
				}
				else if (c >= 'A' && c <= 'Z')
				{
					schemeBuffer += c - 'A' + 'a';
				}
				else if (c == ':')
				{
					url.scheme = std::move(schemeBuffer);
					if (input[pointer + 1] == '/') {
						state = URLParserState::PathOrAuthorityState;
						pointer += 1;
					}
					else {
						url.path = std::string{};
						state = URLParserState::OpaquePathState;
					}
				}
				else
				{
					state = URLParserState::NoSchemeState;
					pointer = -1;
				}
			break;

			case URLParserState::NoSchemeState:
				if (base == std::nullopt)
				{
					throw std::invalid_argument{"missing-scheme-non-relative-URL"};
				}
				else if (std::holds_alternative<std::string>(base->path))
				{
					if (c != '#')
					{
						throw std::invalid_argument{"missing-scheme-non-relative-URL"};
					}
					else
					{
						url.scheme = base->scheme;
						url.path = base->path;
						url.query = base->query;
						url.fragment = std::string{};
						state = URLParserState::FragmentState;
					}
				}
				else
				{
					state = URLParserState::RelativeState;
					pointer -= 1;
				}
			break;

			case URLParserState::PathOrAuthorityState:
				if (c == '/')
				{
					state = URLParserState::AuthorityState;
				}
				else
				{
					state = URLParserState::PathState;
					pointer -= 1;
				}
			break;

			case URLParserState::RelativeState:
				if (base->scheme == "file")
				{
					std::terminate();
				}
				url.scheme = base->scheme;
				if (c == '/')
				{
					state = URLParserState::RelativeSlashState;
				}
				else
				{
					url.username = base->username;
					url.password = base->password;
					url.host = base->host;
					url.port = base->port;
					url.path = base->path;
					url.query = base->query;
					if (c == '?')
					{
						url.query = std::string{};
						state = URLParserState::QueryState;
					}
					else if (c == '#')
					{
						url.fragment = std::string{};
						state = URLParserState::FragmentState;
					}
					else
					{
						url.query = std::nullopt;
						url.path = shorten_path(url.path);
						state = URLParserState::PathState;
						pointer -= 1;
					}
				}
			break;

			case URLParserState::RelativeSlashState:
				if (c == '/')
				{
					state = URLParserState::AuthorityState;
				}
				else
				{
					url.username = base->username;
					url.password = base->password;
					url.host = base->host;
					url.port = base->port;
					state = URLParserState::PathState;
					pointer -= 1;
				}
			break;

			case URLParserState::AuthorityState:
				if (c == '@')
				{
					if (atSignSeen)
					{
						authorityBuffer = U"%40" + authorityBuffer;
					}
					atSignSeen = true;
					for (const auto& codePoint : authorityBuffer)
					{
						if (codePoint == ':' && passwordTokenSeen == false)
						{
							passwordTokenSeen = true;
							continue;
						}
						auto encodedCodePoints = utf8_percent_encode(codePoint, pes_userinfo);
						if (passwordTokenSeen)
						{
							url.password += encodedCodePoints;
						}
						else
						{
							url.username += encodedCodePoints;
						}
					}
					authorityBuffer = {};
				}
				else if (c == '\0' || c == '/' || c == '?' || c == '#')
				{
					if (atSignSeen == true && authorityBuffer.size() == 0)
					{
						throw std::invalid_argument("invalid-credentials");
					}
					pointer -= authorityBuffer.size() + 1;
					authorityBuffer = {};
					state = URLParserState::HostState;
				}
				else
				{
					authorityBuffer += c;
				}
			break;

			case URLParserState::HostState:
				if (c == ':' && !insideBrackets)
				{
					if (hostBuffer.size() == 0)
					{
						throw std::invalid_argument("host-missing");
					}
					url.host = parse_host(std::move(hostBuffer));
					state = URLParserState::PortState;
				}
				else if (c == '\0' || c == '/' || c == '?' || c == '#')
				{
					pointer -= 1;
					url.host = parse_host(std::move(hostBuffer));
					state = URLParserState::PathStartState;
				}
				else
				{
					if (c == '[')
					{
						insideBrackets = true;
					}
					if (c == ']')
					{
						insideBrackets = false;
					}
					hostBuffer += c;
				}
			break;

			case URLParserState::PortState:
				if (c >= '0' and c <= '9')
				{
					portBuffer += c;
				}
				else if (c == '\0' || c == '/' || c == '?' || c == '#')
				{
					if (portBuffer.size())
					{
						auto port = (unsigned long) 0;
						try
						{
							port = std::stoul(portBuffer);
						}
						catch (const std::out_of_range& e)
						{
							throw std::invalid_argument("port-out-of-range");
						}
						catch (const std::invalid_argument&)
						{
							std::terminate();
						}
						if (port > 65535) {
							throw std::invalid_argument("port-out-of-range");
						}
						auto isDefaultPort = false;
						switch (port) {
							case 21:
								if (url.scheme == "ftp") isDefaultPort = true;
								break;
							case 80:
								if (url.scheme == "http") isDefaultPort = true;
								if (url.scheme == "ws") isDefaultPort = true;
								break;
							case 443:
								if (url.scheme == "https") isDefaultPort = true;
								if (url.scheme == "wss") isDefaultPort = true;
								break;
						}
						if (isDefaultPort)
						{
							url.port = std::nullopt;
						}
						else
						{
							url.port = port;
						}
					}
					state = URLParserState::PathStartState;
					pointer -= 1;
				}
				else
				{
					throw std::invalid_argument("port-invalid");
				}
			break;

			case URLParserState::PathStartState:
				if (c == '?')
				{
					url.query = std::string{};
					state = URLParserState::QueryState;
				}
				else if (c == '#')
				{
					url.fragment = std::string{};
					state = URLParserState::FragmentState;
				}
				else if (c != '\0')
				{
					state = URLParserState::PathState;
					if (c != '/') pointer -= 1;
				}
			break;

			case URLParserState::PathState:
				if (c == '\0' || c == '/' || c == '?' || c == '#')
				{
					if (pathSegmentBuffer == ".." || pathSegmentBuffer ==  ".%2e" || pathSegmentBuffer ==  ".%2E" || pathSegmentBuffer == "%2e." || pathSegmentBuffer == "%2E." || pathSegmentBuffer == "%2e%2e" || pathSegmentBuffer == "%2E%2E")
					{
						url.path = shorten_path(url.path);
						if (c != '/')
						{
							std::get<std::vector<std::string>>(url.path).emplace_back(std::string{});
						}
					}
					else if (pathSegmentBuffer == "." || pathSegmentBuffer ==  "%2e" || pathSegmentBuffer ==  "%2E")
					{
						if (c != '/')
						{
							std::get<std::vector<std::string>>(url.path).emplace_back(std::string{});
						}
					}
					else
					{
						std::get<std::vector<std::string>>(url.path).emplace_back(std::move(pathSegmentBuffer));
					}
					pathSegmentBuffer = {};
					if (c == '?')
					{
						url.query = std::string{};
						state = URLParserState::QueryState;
					}
					else if (c == '#')
					{
						url.fragment = std::string{};
						state = URLParserState::FragmentState;
					}
				}
				else
				{
					pathSegmentBuffer += utf8_percent_encode(c, pes_path);
				}
			break;

			case URLParserState::OpaquePathState:
				if (c == '?')
				{
					url.query = std::string{};
					state = URLParserState::QueryState;
				}
				else if (c == '#')
				{
					url.fragment = std::string{};
					state = URLParserState::FragmentState;
				}
				else if (c != '\0')
				{
					std::get<std::string>(url.path) += utf8_percent_encode(c, pes_c0_control);
				}
			break;

			case URLParserState::QueryState:
				if (c == '#')
				{
					url.fragment = std::string{};
					state = URLParserState::FragmentState;
				}
				else if (c != '\0')
				{
					*url.query += utf8_percent_encode(c, pes_query);
				}
			break;

			case URLParserState::FragmentState:
				if (c != '\0')
				{
					*url.fragment += utf8_percent_encode(c, pes_fragment);
				}
		}
	}
}

URL::URL(const std::u32string& input, const std::optional<URL>& base)
{
	parse_url(input, base, *this);
}

URL::URL(const std::string& input, const std::optional<URL>& base)
{
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8conv;
	parse_url(utf8conv.from_bytes((const char*) input.data()), base, *this);
}

auto sparqlxx::URL::href() const -> std::string
{
	auto output = scheme + ":";
	if (host)
	{
		output += "//";
		if (username.size() || password.size())
		{
			output += username;
			if (password.size())
			{
				output += ':' + password;
			}
			output += '@';
		}
		output += *host;
		if (port)
		{
			output += ':';
			output += std::to_string(*port);
		}
	}

	if (host == std::nullopt && !std::holds_alternative<std::string>(path) && std::get<std::vector<std::string>>(path).size() > 1 && std::get<std::vector<std::string>>(path)[0].size() == 0)
	{
		output += "/.";
	}
	if (std::holds_alternative<std::string>(path))
	{
		output += std::get<std::string>(path);
	}
	else
	{
		for (const auto& segment : std::get<std::vector<std::string>>(path))
		{
			output += '/';
			output += segment;
		}
	}

	if (query)
	{
		output += '?';
		output += *query;
	}

	if (fragment)
	{
		output += '#';
		output += *fragment;
	}

	return output;
}
}
