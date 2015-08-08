#pragma once

#include "../parse_error.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "../atoms.h"

namespace sparqlxx
{
	using std::string;
	using std::vector;
	using std::unordered_map;

	inline bool iequals(const string& a, const string& b)
	{
		if (b.size() != a.size())
			return false;
		for (size_t i = 0; i < a.size(); ++i)
			if (tolower(a[i]) != tolower(b[i]))
				return false;
			return true;
	}

	template <typename T>
	struct range
	{
		T i;
		T end;

		range(T i, T end): i(i), end(end) {}

		auto operator++() -> range& { ++i; return *this; }
		auto operator++(int) -> range { auto x = *this; ++i; return x; }
		auto operator*() { return *i; }
		auto operator*() const { return *i; }
		auto operator->() { return i.operator->(); }
		auto operator->() const { return i.operator->(); }

		auto operator!() const { return i == end; }
		operator bool() const { return !!(*this); }
	};

	template <typename T>
	struct debug_range
	{
		T i;
		std::vector<std::string> expected;

		template <typename... Arg>
		debug_range(Arg... arg): i(std::forward<Arg>(arg)...) {}

		auto expect(std::string x) {expected.push_back(x);}

		auto operator++() -> debug_range& { ++i; expected = {}; return *this; }
		auto operator++(int) -> debug_range { auto x = *this; ++i; expected = {}; return x; }
		auto operator*() { return *i; }
		auto operator*() const { return *i; }
		auto operator->() { return i.operator->(); }
		auto operator->() const { return i.operator->(); }

		auto operator!() const { return !i; }
		operator bool() const { return !!(*this); }
	};

	struct token_stream
	{
		debug_range<range<vector<string>::const_iterator>> token;

		Iri base;
		unordered_map<string, Iri> prefixes;

		auto error()
		{
			return parse_error{std::move(token.expected), token ? *token : "end-of-input"};
		}

		auto is_end()
		{
			return !token;
		}

		auto is(const char* code)
		{
			token.expect(code);
			return !is_end() && iequals(*token, code);
		}

		auto match_end()
		{
			token.expect("end");
			if (!is_end())
				throw error();
		}

		auto match(const char* code)
		{
			if (!is(code))
				throw error();
			++token;
		}

		auto try_match(const char* code) -> bool
		{
			if (!is(code))
				return false;
			++token;
			return true;
		}

		auto read_label() -> string
		{
			token.expect("label");
			if (is_end())
				throw error();
			return *(token++);
		}

		template <typename T>
		auto is() -> bool;

		template <typename T>
		auto read() -> T;

		template <typename T>
		auto try_read() -> optional<T>
		{
			if (is<T>())
				return read<T>();
			else
				return nullopt;
		}

		token_stream(const vector<string>& tokens): token{std::cbegin(tokens), std::cend(tokens)} {}
	};
}
