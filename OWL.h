#pragma once

#define _nsowl "http://www.w3.org/2002/07/owl#"

// http://www.w3.org/2002/07/owl# namespace
namespace OWL
{
	constexpr const auto Class           = _nsowl "Class";
	constexpr const auto DeprecatedClass = _nsowl "DeprecatedClass";

	constexpr const auto Ontology = _nsowl "Ontology";

	constexpr const auto Thing   = _nsowl "Thing";
	constexpr const auto Nothing = _nsowl "Nothing";

	constexpr const auto ObjectProperty            = _nsowl "ObjectProperty";
	constexpr const auto DatatypeProperty          = _nsowl "DatatypeProperty";
	constexpr const auto TransitiveProperty        = _nsowl "TransitiveProperty";
	constexpr const auto SymmetricProperty         = _nsowl "SymmetricProperty";
	constexpr const auto FunctionalProperty        = _nsowl "FunctionalProperty";
	constexpr const auto InverseFunctionalProperty = _nsowl "InverseFunctionalProperty";
	constexpr const auto AnnotationProperty        = _nsowl "AnnotationProperty";
	constexpr const auto OntologyProperty          = _nsowl "OntologyProperty";
	constexpr const auto DeprecatedProperty        = _nsowl "DeprecatedProperty";

	constexpr const auto DataRange                 = _nsowl "DataRange";

	constexpr const auto equivalentClass    = _nsowl "equivalentClass";
	constexpr const auto disjointWith       = _nsowl "disjointWith";
	constexpr const auto equivalentProperty = _nsowl "equivalentProperty";
	constexpr const auto sameAs             = _nsowl "sameAs";
	constexpr const auto differentFrom      = _nsowl "differentFrom";
	constexpr const auto AllDifferent       = _nsowl "AllDifferent";
	constexpr const auto distinctMembers    = _nsowl "distinctMembers";
	constexpr const auto unionOf            = _nsowl "unionOf";
	constexpr const auto intersectionOf     = _nsowl "intersectionOf";
	constexpr const auto complementOf       = _nsowl "complementOf";
	constexpr const auto oneOf              = _nsowl "oneOf";
	constexpr const auto inverseOf          = _nsowl "inverseOf";

	constexpr const auto Restriction        = _nsowl "Restriction";
	constexpr const auto onProperty         = _nsowl "onProperty";
	constexpr const auto allValuesFrom      = _nsowl "allValuesFrom";
	constexpr const auto hasValue           = _nsowl "hasValue";
	constexpr const auto someValuesFrom     = _nsowl "someValuesFrom";
	constexpr const auto minCardinality     = _nsowl "minCardinality";
	constexpr const auto maxCardinality     = _nsowl "maxCardinality";
	constexpr const auto cardinality        = _nsowl "cardinality";

	constexpr const auto versionInfo            = _nsowl "versionInfo";
	constexpr const auto imports                = _nsowl "imports";
	constexpr const auto priorVersion           = _nsowl "priorVersion";
	constexpr const auto backwardCompatibleWith = _nsowl "backwardCompatibleWith";
	constexpr const auto incompatibleWith       = _nsowl "incompatibleWith";
}
