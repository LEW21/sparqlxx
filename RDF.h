#pragma once

#define _nsrdf "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define _nsrdfs "http://www.w3.org/2000/01/rdf-schema#"

// Sum of http://www.w3.org/1999/02/22-rdf-syntax-ns# (RDF) and http://www.w3.org/2000/01/rdf-schema# (RDFS) namespaces.
namespace RDF
{
	//- 2. Classes
	constexpr const auto Resource  = _nsrdfs "Resource";
	constexpr const auto Class     = _nsrdfs "Class";
	constexpr const auto Literal   = _nsrdfs "Literal";
	constexpr const auto Datatype  = _nsrdfs "Datatype";
	constexpr const auto langString= _nsrdf  "langString";
	constexpr const auto HTML      = _nsrdf  "HTML";
	constexpr const auto XMLLiteral= _nsrdf  "XMLLiteral";
	constexpr const auto Property  = _nsrdf  "Property";

	//- 3. Properties
	constexpr const auto range        = _nsrdfs "range";
	constexpr const auto domain       = _nsrdfs "domain";
	constexpr const auto type         = _nsrdf  "type";
	constexpr const auto subClassOf   = _nsrdfs "subClassOf";
	constexpr const auto subPropertyOf= _nsrdfs "subPropertyOf";
	constexpr const auto label        = _nsrdfs "label";
	constexpr const auto comment      = _nsrdfs "comment";

	//- 5.1 Container Classes and Properties
	constexpr const auto Container = _nsrdfs "Container";
	constexpr const auto Bag       = _nsrdf  "Bag";
	constexpr const auto Seq       = _nsrdf  "Seq";
	constexpr const auto Alt       = _nsrdf  "Alt";
	constexpr const auto ContainerMembershipProperty= _nsrdfs "ContainerMembershipProperty";
	constexpr const auto member    = _nsrdfs "member";

	//- 5.2 RDF Collections
	constexpr const auto List      = _nsrdf  "List";
	constexpr const auto first     = _nsrdf  "first";
	constexpr const auto rest      = _nsrdf  "rest";
	constexpr const auto nil       = _nsrdf  "nil";

	//- 5.3 Reification Vocabulary
	constexpr const auto Statement = _nsrdf  "Statement";
	constexpr const auto subject   = _nsrdf  "subject";
	constexpr const auto predicate = _nsrdf  "predicate";
	constexpr const auto object    = _nsrdf  "object";

	//- 5.4 Utility Properties
	constexpr const auto seeAlso    = _nsrdfs "seeAlso";
	constexpr const auto isDefinedBy= _nsrdfs "isDefinedBy";
	constexpr const auto value      = _nsrdf  "value";
}
