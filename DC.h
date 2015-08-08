#pragma once

#include <string>
#include "atoms.h"

namespace DC
{
	const auto _ns = std::string{"http://purl.org/dc/terms/"};

	// Classes
	const auto Agent                        = sparqlxx::Iri{_ns + "Agent"};
	const auto AgentClass                   = sparqlxx::Iri{_ns + "AgentClass"};
	const auto BibliographicResource        = sparqlxx::Iri{_ns + "BibliographicResource"};
	const auto FileFormat                   = sparqlxx::Iri{_ns + "FileFormat"};
	const auto Frequency                    = sparqlxx::Iri{_ns + "Frequency"};
	const auto Jurisdiction                 = sparqlxx::Iri{_ns + "Jurisdiction"};
	const auto LicenseDocument              = sparqlxx::Iri{_ns + "LicenseDocument"};
	const auto LinguisticSystem             = sparqlxx::Iri{_ns + "LinguisticSystem"};
	const auto Location                     = sparqlxx::Iri{_ns + "Location"};
	const auto LocationPeriodOrJurisdiction = sparqlxx::Iri{_ns + "LocationPeriodOrJurisdiction"};
	const auto MediaType                    = sparqlxx::Iri{_ns + "MediaType"};
	const auto MediaTypeOrExtent            = sparqlxx::Iri{_ns + "MediaTypeOrExtent"};
	const auto MethodOfAccrual              = sparqlxx::Iri{_ns + "MethodOfAccrual"};
	const auto MethodOfInstruction          = sparqlxx::Iri{_ns + "MethodOfInstruction"};
	const auto PeriodOfTime                 = sparqlxx::Iri{_ns + "PeriodOfTime"};
	const auto PhysicalMedium               = sparqlxx::Iri{_ns + "PhysicalMedium"};
	const auto PhysicalResource             = sparqlxx::Iri{_ns + "PhysicalResource"};
	const auto Policy                       = sparqlxx::Iri{_ns + "Policy"};
	const auto ProvenanceStatement          = sparqlxx::Iri{_ns + "ProvenanceStatement"};
	const auto RightsStatement              = sparqlxx::Iri{_ns + "RightsStatement"};
	const auto SizeOrDuration               = sparqlxx::Iri{_ns + "SizeOrDuration"};
	const auto Standard                     = sparqlxx::Iri{_ns + "Standard"};
}
