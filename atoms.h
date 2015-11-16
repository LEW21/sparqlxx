#pragma once

#include <string>
#include <vector>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include <network/uri.hpp>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop
#include <boost/optional.hpp>

namespace sparqlxx
{
	using std::string;
	using std::vector;
	using boost::optional;
	using boost::make_optional;
	/*constexpr*/ const auto nullopt = boost::none;

	struct [[gnu::visibility("default")]] Var
	{
		std::string name;

		Var() {}
		Var(const std::string& name): name(name) {}
		Var(std::string&& name): name(std::move(name)) {}
	};

	inline bool operator==(const Var& a, const Var& b) { return a.name == b.name; }
	inline bool operator!=(const Var& a, const Var& b) { return !(a == b); }

	inline auto to_string(const Var& v) -> std::string
	{
		return v.name;
	}

	inline auto to_sparql(const Var& v) -> std::string
	{
		return '?' + v.name;
	}

	inline auto _parse_uri(const std::string& iri) -> network::uri
	{
		try
		{
			return network::uri{iri};
		}
		// It really sux we need to do this.
		catch (network::uri_syntax_error&)
		{
			return network::uri_builder().path(iri).uri();
		}
	}

	struct [[gnu::visibility("default")]] Iri
	{
		network::uri iri;

		Iri() {}
		Iri(const std::string& iri): iri(_parse_uri(iri)) {}
		Iri(std::string&& iri): iri(_parse_uri(std::move(iri))) {}

		explicit Iri(const network::uri& iri): iri(iri) {}
		explicit Iri(network::uri&& iri): iri(std::move(iri)) {}

		auto resolve(const Iri& base) -> Iri { return Iri{iri.resolve(base.iri)}; }
	};

	inline auto to_string(const Iri& v) -> std::string
	{
		return v.iri.to_string<char>();
	}

	inline auto to_sparql(const Iri& v) -> std::string
	{
		// Note from 4.1.1 Syntax for IRIs:
		// RDF URI references containing "<", ">", '"' (double quote), space, "{", "}", "|", "\", "^", and "`" are not IRIs.
		// The behavior of a SPARQL query against RDF statements composed of such RDF URI references is not defined.
		return "<" + to_string(v) + ">";
	}

	// Note: network::uri sux, operator== crashes if any IRI is invalid (even when they're ==).
	inline bool operator==(const Iri& a, const Iri& b) { return to_string(a) == to_string(b); }
	inline bool operator!=(const Iri& a, const Iri& b) { return !(a == b); }

	inline bool operator==(const Iri& a, const char* b) { return to_string(a) == b; }
	inline bool operator!=(const Iri& a, const char* b) { return !(a == b); }

	struct [[gnu::visibility("default")]] LangTag
	{
		std::string tag;

		LangTag() {}
		LangTag(const std::string& tag): tag{tag} {}
		LangTag(std::string&& tag): tag{std::move(tag)} {}
	};

	inline bool operator==(const LangTag& a, const LangTag& b) { return a.tag == b.tag; }
	inline bool operator!=(const LangTag& a, const LangTag& b) { return !(a == b); }

	struct [[gnu::visibility("default")]] Literal
	{
		std::string value;
		Iri type;
		LangTag lang;

		Literal() {}
		explicit Literal(const std::string& value): value{value}, type{"http://www.w3.org/2001/XMLSchema#string"} {}
		Literal(const std::string& value, const Iri& type): value{value}, type{type} {}
		Literal(const std::string& value, const LangTag& lang): value{value}, type{"http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"}, lang{lang} {}

		bool isString() const { return type == "http://www.w3.org/2001/XMLSchema#string"; }
		bool isLangString() const { return type == "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"; }
	};

	inline bool operator==(const Literal& a, const Literal& b) { return a.value == b.value && a.type == b.type && a.lang == b.lang; }
	inline bool operator!=(const Literal& a, const Literal& b) { return !(a == b); }

	inline auto to_string(const Literal& v) -> std::string
	{
		return v.value;
	}

	inline auto encode_langstring(const Literal& v) -> std::string
	{
		return to_string(v) + "@" + v.lang.tag;
	}

	inline auto decode_langstring(const std::string& s) -> Literal
	{
		auto lang_pos = s.find_last_of('@');
		if (lang_pos == std::string::npos)
			throw std::invalid_argument("decode_langstring");
		return Literal{s.substr(0, lang_pos), LangTag{s.substr(lang_pos + 1)}};
	}

	inline auto __quote(const std::string& v)
	{
		// 19.7 Escape sequences in strings
		auto turtle = std::string{};
		turtle.reserve(v.size() * 2);

		turtle += '"';

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

				case '\b':
					turtle += "\\b";
					break;

				case '\f':
					turtle += "\\f";
					break;

				case '"':
					turtle += "\\\"";
					break;

				default:
					turtle += c;
					break;
			}

			turtle += '"';
			return turtle;
	}

	inline auto to_sparql(const Literal& v) -> std::string
	{
		auto s = __quote(to_string(v));
		if (v.type == Iri{"http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"})
			return s + "@" + v.lang.tag;
		else if (v.type == Iri{"http://www.w3.org/2001/XMLSchema#string"})
			return s;
		else
			return s + "^^" + to_sparql(v.type);
	}

	struct [[gnu::visibility("default")]] BlankNode
	{
		std::string name;

		BlankNode() {}
		BlankNode(const std::string& name): name(name) {}
		BlankNode(std::string&& name): name(std::move(name)) {}

		auto is_anon() const { return name.size() == 0; }
	};

	inline bool operator==(const BlankNode& a, const BlankNode& b) { return a.name == b.name; }
	inline bool operator!=(const BlankNode& a, const BlankNode& b) { return !(a == b); }

	inline auto to_string(const BlankNode& b) -> std::string
	{
		return "_:" + b.name;
	}

	inline auto to_sparql(const BlankNode& b) -> std::string
	{
		return "_:" + b.name;
	}

	struct [[gnu::visibility("default")]] Prefix
	{
		std::string name;

		Prefix() {}
		Prefix(const std::string& name): name(name) {}
		Prefix(std::string&& name): name(std::move(name)) {}
	};
}
