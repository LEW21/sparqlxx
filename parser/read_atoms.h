#pragma once

#include "../atoms.h"
#include "token_stream.h"

namespace sparqlxx
{
	inline auto is_valid_text(const std::string& t) -> bool
	{
		for (auto c : t)
			if (c <= 0x1F && c != '\t' && c != '\n' && c != '\v' && c != '\r')
				return false;

		return true;
	}

	template<>
	inline auto token_stream::is<int>() -> bool
	{
		token.expect("int");
		return !is_end() && true; // TODO
	}

	template<>
	inline auto token_stream::read<int>() -> int
	{
		if (!is<int>())
			throw error();

		return std::stoi(*token++);
	}

	template<>
	inline auto token_stream::is<Var>() -> bool
	{
		token.expect("Var");
		return !is_end() && ((*token)[0] == '$' || (*token)[0] == '?') && token->size() > 1 && is_valid_text(*token);
	}

	template<>
	inline auto token_stream::read<Var>() -> Var
	{
		if (!is<Var>())
			throw error();

		return Var{(*token++).substr(1)};
	}

	const auto is_multi_byte = char(0x80);

	template<>
	inline auto token_stream::is<Iri>() -> bool
	{
		token.expect("Iri");
		if (is_end())
			return false;

		if (!is_valid_text(*token))
			return false;

		if ((*token)[0] == '<' && (*token)[(*token).size() - 1] == '>')
			return true;

		for (const auto& pair : prefixes)
			if (token->size() > (pair.first.size()+1) && token->compare(0, pair.first.size(), pair.first) == 0 && (*token)[pair.first.size()] == ':')
				return true;

		return false;
	}

	template<>
	inline auto token_stream::read<Iri>() -> Iri
	{
		if (!is<Iri>())
			throw error();

		auto t = *token++;

		if (t[0] == '<') // IRIREF
			return Iri{t.substr(1, t.size() - 2)}.resolve(base);

		// PrefixedName
		auto colon_pos = t.find(':');
		auto prefix = to_string(prefixes.at(t.substr(0, colon_pos)));
		return Iri{prefix + t.substr(colon_pos + 1)};
	}

	template<>
	inline auto token_stream::is<Prefix>() -> bool
	{
		token.expect("Prefix");
		if (is_end())
			return false;

		auto first = (*token)[0];
		if (!(first & is_multi_byte || (first >= 'a' && first <= 'z') || (first >= 'A' && first <= 'Z')))
			return false;

		auto got_colon = false;
		for (auto c : *token)
		{
			if (got_colon)
				return false;

			if (c == ':')
				got_colon = true;
		}
		return got_colon;
	}

	template<>
	inline auto token_stream::read<Prefix>() -> Prefix
	{
		if (!is<Prefix>())
			throw error();

		auto prefix = *token++;
		return Prefix{prefix.substr(0, prefix.size() - 1)};
	}

	template<>
	inline auto token_stream::is<BlankNode>() -> bool
	{
		token.expect("BlankNode");
		if (is_end())
			return false;

		if (!is_valid_text(*token))
			return false;

		return token->compare(0, 2, "_:") == 0;
	}

	template<>
	inline auto token_stream::read<BlankNode>() -> BlankNode
	{
		if (!is<BlankNode>())
			throw error();

		auto t = *token++;
		return BlankNode{t.substr(2)};
	}

	template<>
	inline auto token_stream::is<LangTag>() -> bool
	{
		token.expect("LangTag");
		if (is_end())
			return false;

		const auto& t = *token;
		if (t[0] == '@')
			return true;

		return false;
	}

	template<>
	inline auto token_stream::read<LangTag>() -> LangTag
	{
		if (!is<LangTag>())
			throw error();

		auto t = *token++;

		return LangTag{t.substr(1)};
	}

	template<>
	inline auto token_stream::is<Literal>() -> bool
	{
		token.expect("Literal");
		if (is_end())
			return false;

		if (!is_valid_text(*token))
			return false;

		const auto& t = *token;
		if (t[0] == '"' || t[0] == '\'')
			return true;

		return false;
	}

	template<>
	inline auto token_stream::read<Literal>() -> Literal
	{
		if (!is<Literal>())
			throw error();

		auto t = *token++;

		auto num_quotes =
			(t.compare(0, 3, "'''") == 0 || t.compare(0, 3, "\"\"\"") == 0) ? 3 :
			(t[0] == '\'' || t[0] == '"') ? 1 :
			0;

		auto val = t.substr(num_quotes, t.size() - 2 * num_quotes);

		// TODO unescape slashes

		if (try_match("^^"))
		{
			auto type = read<Iri>();
			return Literal{val, type};
		}
		else if (is<LangTag>())
		{
			auto lang = read<LangTag>();
			return Literal{val, lang};
		}
		else
			return Literal{val};
	}
}
