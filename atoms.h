#pragma once

#include <string>
#include <vector>
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

	struct [[gnu::visibility("default")]] Iri
	{
		std::string iri;

		Iri() {}
		Iri(const std::string& iri): iri(iri) {}
		Iri(std::string&& iri): iri(std::move(iri)) {}
	};

	inline auto to_string(const Iri& v) -> std::string
	{
		return v.iri;
	}

	inline auto to_sparql(const Iri& v) -> std::string
	{
		// Note from 4.1.1 Syntax for IRIs:
		// RDF URI references containing "<", ">", '"' (double quote), space, "{", "}", "|", "\", "^", and "`" are not IRIs.
		// The behavior of a SPARQL query against RDF statements composed of such RDF URI references is not defined.
		return "<" + to_string(v) + ">";
	}

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

	/* Encode a rdf:langString as "value@lang".
	 * @v rdf:langString
	 *
	 * As described in http://www.w3.org/TR/rdf-plain-literal/#Definition_of_the_rdf:PlainLiteral_Datatype
	 *
	 * @return string of the form "value@lang"
	 */
	inline auto encode_langstring(const Literal& v) -> std::string
	{
		return to_string(v) + "@" + v.lang.tag;
	}

	/* Decode "value@lang" to a rdf:langString.
	 * @s string of the form "value@lang".
	 *
	 * As described in http://www.w3.org/TR/rdf-plain-literal/#Definition_of_the_rdf:PlainLiteral_Datatype
	 *
	 * @return rdf:langString
	 */
	inline auto decode_langstring(const std::string& s) -> Literal
	{
		auto lang_pos = s.find_last_of('@');
		if (lang_pos == std::string::npos)
			throw std::invalid_argument("decode_langstring");
		return Literal{s.substr(0, lang_pos), LangTag{s.substr(lang_pos + 1)}};
	}

	#ifdef SPARQLXX_DOC
	#define _quote(x) x
	#else
	inline auto _quote(const std::string& v)
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
	#endif

	inline auto to_sparql(const Literal& v) -> std::string
	{
		auto s = _quote(to_string(v));
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
