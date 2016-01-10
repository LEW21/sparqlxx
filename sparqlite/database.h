#pragma once

#include "../databaseimpl.h"

#include "resources.h"
#include "statements.h"

namespace sparqlite
{
	class [[gnu::visibility("default")]] Database: public sparqlxx::DatabaseImpl
	{
		std::string path;

		void load();
		void save();

	public:
		Resources res;
		Statements stmt;

		Database(const char* dsn = nullptr);
		auto query(const sparqlxx::SPARQL&) -> sparqlxx::Result override;

		template <typename T>
		auto _query(const T&) const -> sparqlxx::result_of<T>;

		template <typename T>
		auto _update(const T&) -> sparqlxx::result_of<T>;

		auto match(const sparqlxx::QuadsVP&) const -> sparqlxx::Solutions;
	};
}

extern "C" [[gnu::visibility("default")]] auto sparqlite_init_db(const char* iri) -> sparqlxx::DatabaseImpl*;

namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, sparqlite::Database& r, const unsigned int)
		{
			ar & r.res;
			ar & r.stmt;
		}
	}
}
