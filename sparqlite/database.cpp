#include "database.h"

#include <iostream>

namespace sparqlite
{
	Database::Database() {}

	auto Database::query(const sparqlxx::SPARQL& q) -> sparqlxx::Result
	{
		if (q.is<sparqlxx::Select>())
			return _query(q.get<sparqlxx::Select>());

		if (q.is<sparqlxx::Ask>())
			return _query(q.get<sparqlxx::Ask>());

		if (q.is<sparqlxx::Describe>())
			return _query(q.get<sparqlxx::Describe>());

		if (q.is<sparqlxx::InsertData>())
			return _query(q.get<sparqlxx::InsertData>());

		if (q.is<sparqlxx::DeleteData>())
			return _query(q.get<sparqlxx::DeleteData>());

		return sparqlxx::None{};
	}
}

auto sparqlite_init_db(const char*) -> sparqlxx::DatabaseImpl*
{
	return new sparqlite::Database();
}
