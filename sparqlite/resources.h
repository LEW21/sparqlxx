#pragma once

#include "../atoms.h"
#include "../parse.h"
#include "quad.h"
#include <unordered_map>

#include "R.h"

namespace sparqlite
{
	struct Resource
	{
		ID type; // Datatype / id of any IRI from the resources set.
		std::string value;

		Resource(): type(0) {}

		Resource(ID type, const std::string& value): type(type), value(value) {}
		Resource(ID type, std::string&& value): type(type), value(std::move(value)) {}

		// is not a blank node?
		operator bool() { return type != 0; }
	};

	// Every blank node is different.
	inline auto operator==(const Resource& a, const Resource& b) { return a.type != 0 && a.type == b.type && a.value == b.value; }
	inline auto operator!=(const Resource& a, const Resource& b) { return !(a == b); }
}

namespace std
{
	template<>
	struct hash<sparqlite::Resource>
	{
		std::hash<std::string> vhash;

		typedef sparqlite::Resource argument_type;
		typedef std::size_t result_type;

		inline result_type operator()(const sparqlite::Resource& r) const
		{
			return result_type(r.type) ^ vhash(r.value);
		}
	};
}

namespace sparqlite
{
	class Resources
	{
		std::vector<Resource> byID;
		std::unordered_map<Resource, ID> byRes;

	public:
		inline Resources();

		inline auto operator[](ID id) const -> Resource {return byID[id];}
		inline auto operator[](const Resource& r) const -> ID {return byRes.at(r);}
		inline auto operator[](const Resource&) -> ID;

		inline auto blank() -> ID;

		inline auto operator[](const Quad&) const -> ID;
		inline auto operator[](const Quad&) -> ID;

		inline auto get(ID type, const std::string& value) const -> ID {return operator[](Resource{type, value});}
		inline auto get(ID type, std::string&& value) const -> ID {return operator[](Resource{type, std::move(value)});}
		inline auto get(ID type, const std::string& value) -> ID {return operator[](Resource{type, value});}
		inline auto get(ID type, std::string&& value) -> ID {return operator[](Resource{type, std::move(value)});}

		inline auto operator[](const sparqlxx::Iri& iri) const -> ID { return get(Datatype::iri, to_string(iri)); }
		inline auto operator[](const sparqlxx::Literal&) const -> ID;
		inline auto operator[](const sparqlxx::BlankNode&) const -> ID { return 0; }
		inline auto operator[](const sparqlxx::Var&) const -> ID { return 0; }
		inline auto operator[](const sparqlxx::Term&) const -> ID;
		inline auto operator[](const sparqlxx::VarOrTerm&) const -> ID;

		inline auto operator[](const sparqlxx::Iri& iri) -> ID { return get(Datatype::iri, to_string(iri)); }
		inline auto operator[](const sparqlxx::Literal&) -> ID;
		inline auto operator[](const sparqlxx::BlankNode&) -> ID { return 0; }
		inline auto operator[](const sparqlxx::Var&) -> ID { return 0; }
		inline auto operator[](const sparqlxx::Term&) -> ID;
		inline auto operator[](const sparqlxx::VarOrTerm&) -> ID;

		inline auto toIri(ID id) const -> sparqlxx::Iri;
		inline auto toTerm(ID id) const -> sparqlxx::Term;
		inline auto toQuad(ID id) const -> sparqlxx::Quad;
		inline auto toTriple(ID id) const -> sparqlxx::Triple;

		inline auto toLiteQuad(ID id) const -> Quad;
		inline auto toLiteTriple(ID id) const -> Triple;

		inline auto toSPARQL(ID id) const -> std::string;
	};

	inline Resources::Resources()
		: byID{
			Resource{},
			Resource{Datatype::iri, RIRI::iri},
			Resource{Datatype::iri, RIRI::string},
			Resource{Datatype::iri, RIRI::langString},
			Resource{Datatype::iri, RIRI::statement},

			// RDFS: 3. Properties
			Resource{Datatype::iri, RIRI::range},
			Resource{Datatype::iri, RIRI::domain},
			Resource{Datatype::iri, RIRI::type},
			Resource{Datatype::iri, RIRI::subClassOf},
			Resource{Datatype::iri, RIRI::subPropertyOf},
			Resource{Datatype::iri, RIRI::label},
			Resource{Datatype::iri, RIRI::comment},

			// OWL properties
			Resource{Datatype::iri, RIRI::sameAs},
			Resource{Datatype::iri, RIRI::inverseOf},
			Resource{Datatype::iri, RIRI::equivalentProperty},
			Resource{Datatype::iri, RIRI::equivalentClass},

			// OWL types
			Resource{Datatype::iri, RIRI::TransitiveProperty},
			Resource{Datatype::iri, RIRI::SymmetricProperty},

			// Xtreeme meta
			Resource{Datatype::iri, RIRI::source},
			Resource{Datatype::iri, RIRI::inferredFrom},
			Resource{Datatype::iri, RIRI::Insertion},
			Resource{Datatype::iri, RIRI::Inference},

			Resource{Datatype::iri, RIRI::DefaultGraphNode},
		}
	{
		for (auto i = 1; i < byID.size(); ++i)
			byRes[byID[i]] = i;
	}

	inline auto Resources::operator[](const Resource& r) -> ID
	{
		try
		{
			return byRes.at(r);
		}
		catch (std::out_of_range&)
		{
			auto id = ID(byID.size());
			byID.push_back(r);
			byRes[r] = id;
			return id;
		}
	}

	inline auto Resources::blank() -> ID
	{
		auto id = ID(byID.size());
		byID.emplace_back();
		return id;
	}

	inline auto Resources::operator[](const Quad& q) const -> ID
	{
		return get(Datatype::statement, toSPARQL(q.subject) + " " + toSPARQL(q.predicate) + " " + toSPARQL(q.object) + " " + toSPARQL(q.graph));
	}

	inline auto Resources::operator[](const Quad& q) -> ID
	{
		return get(Datatype::statement, toSPARQL(q.subject) + " " + toSPARQL(q.predicate) + " " + toSPARQL(q.object) + " " + toSPARQL(q.graph));
	}

	inline auto Resources::operator[](const sparqlxx::Literal& l) const -> ID
	{
		if (l.isLangString())
			return get(Datatype::langString, sparqlxx::encode_langstring(l));
		else if (l.isString())
			return get(Datatype::string, to_string(l));
		return get(operator[](l.type), to_string(l));
	}

	inline auto Resources::operator[](const sparqlxx::Literal& l) -> ID
	{
		if (l.isLangString())
			return get(Datatype::langString, sparqlxx::encode_langstring(l));
		else if (l.isString())
			return get(Datatype::string, to_string(l));
		return get(operator[](l.type), to_string(l));
	}

	inline auto Resources::operator[](const sparqlxx::Term& t) const -> ID
	{
		return t.call([this](const auto& v){return (*this)[v];});
	}

	inline auto Resources::operator[](const sparqlxx::Term& t) -> ID
	{
		return t.call([this](const auto& v){return (*this)[v];});
	}

	inline auto Resources::operator[](const sparqlxx::VarOrTerm& t) const -> ID
	{
		return t.call([this](const auto& v){return (*this)[v];});
	}

	inline auto Resources::operator[](const sparqlxx::VarOrTerm& t) -> ID
	{
		return t.call([this](const auto& v){return (*this)[v];});
	}

	inline auto Resources::toIri(ID id) const -> sparqlxx::Iri
	{
		const auto& r = operator[](id);
		if (r.type != Datatype::iri)
			throw std::invalid_argument("toIri");
		return {r.value};
	}

	inline auto Resources::toTerm(ID id) const -> sparqlxx::Term
	{
		const auto& r = operator[](id);
		if (!r.type)
			return sparqlxx::BlankNode{};
		if (r.type == Datatype::iri)
			return sparqlxx::Iri{r.value};
		if (r.type == Datatype::langString)
			return sparqlxx::decode_langstring(r.value);
		return sparqlxx::Literal{r.value, toIri(r.type)};
	}

	inline auto Resources::toQuad(ID id) const -> sparqlxx::Quad
	{
		const auto& r = operator[](id);
		if (r.type != Datatype::statement)
			throw std::invalid_argument("toQuad");
		return sparqlxx::parse_quad(r.value);
	}

	inline auto Resources::toLiteQuad(ID id) const -> Quad
	{
		auto xq = toQuad(id);
		return Quad{(*this)[xq.subject], (*this)[xq.predicate], (*this)[xq.object], (*this)[xq.graph]};
	}

	inline auto Resources::toTriple(ID id) const -> sparqlxx::Triple
	{
		auto q = toQuad(id);
		return {q.subject, q.predicate, q.object};
	}

	inline auto Resources::toLiteTriple(ID id) const -> Triple
	{
		auto q = toLiteQuad(id);
		return {q.subject, q.predicate, q.object};
	}

	inline auto Resources::toSPARQL(ID id) const -> std::string
	{
		const auto& r = operator[](id);
		if (!r.type)
			return std::string{"_:x"} + std::to_string(id);
		return to_sparql(toTerm(id));
	}
}
