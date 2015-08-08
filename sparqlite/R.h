#pragma once

#include "../RDF.h"
#include "../OWL.h"
#include "../XSD.h"

namespace sparqlite
{
	namespace R
	{
		enum R
		{
			Null = 0,

			iri = 1,
			string = 2, // xsd:string
			langString = 3, // rdf:langString; note that @lang is stored in the value like in rdf:PlainLiteral
			statement = 4,

			// RDFS: 3. Properties
			range,
			domain,
			type,
			subClassOf,
			subPropertyOf,
			label,
			comment,

			// OWL properties
			sameAs,
			inverseOf,
			equivalentProperty,
			equivalentClass,

			// OWL types
			TransitiveProperty,
			SymmetricProperty,

			// Xtreeme meta
			source,
			inferredFrom,
			Insertion,
			Inference,

			// ARQ
			DefaultGraphNode
		};
	}

	namespace RIRI
	{
		constexpr const auto iri = "http://xtreeme.org/#iri";
		using XSD::string;
		using RDF::langString;
		constexpr const auto statement = "http://xtreeme.org/#statement";

		using RDF::range;
		using RDF::domain;
		using RDF::type;
		using RDF::subClassOf;
		using RDF::subPropertyOf;
		using RDF::label;
		using RDF::comment;

		using OWL::sameAs;
		using OWL::inverseOf;
		using OWL::equivalentProperty;
		using OWL::equivalentClass;

		using OWL::TransitiveProperty;
		using OWL::SymmetricProperty;

		constexpr const auto source = "http://xtreeme.org/#source";
		constexpr const auto inferredFrom = "http://xtreeme.org/#inferredFrom";
		constexpr const auto Insertion = "http://xtreeme.org/#Insertion";
		constexpr const auto Inference = "http://xtreeme.org/#Inference";

		constexpr const auto DefaultGraphNode = "urn:x-arq:DefaultGraphNode";
	}

	namespace Datatype
	{
		enum Datatype
		{
			iri = 1,
			string = 2, // xsd:string
			langString = 3, // rdf:langString; note that @lang is stored in the value like in rdf:PlainLiteral
			statement = 4
		};
	}

	namespace DatatypeIRI
	{
		using namespace RIRI;
	}

}
