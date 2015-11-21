#pragma once

#include "../databaseimpl.h"

#include "resources.h"
#include "statements.h"

namespace sparqlite
{
	struct [[gnu::visibility("default")]] Database: public sparqlxx::DatabaseImpl
	{
		Resources res;
		Statements stmt;

		Database();
		auto query(const sparqlxx::SPARQL&) -> sparqlxx::Result override;

		template <typename T>
		auto _query(const T&) -> sparqlxx::result_of<T>;

		auto match(const sparqlxx::QuadsVP&) const -> sparqlxx::Solutions;
	};
}

extern "C" [[gnu::visibility("default")]] auto sparqlite_init_db(const char* iri) -> sparqlxx::DatabaseImpl*;
