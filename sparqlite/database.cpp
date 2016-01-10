#include "database.h"

#include <fstream>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace sparqlite
{
	Database::Database(const char* dsn)
	{
		if (!dsn || dsn[0] == '\0')
			return;

		// dsn shall be file:///path
		auto uri = std::string{dsn};
		if (uri.substr(0, 8) != "file:///")
			throw std::invalid_argument{"Database()"};

		path = uri.substr(7);
		load();
	}

	void Database::load()
	{
		if (!path.size())
			return;

		std::ifstream is{path, std::ofstream::in | std::ofstream::binary};
		if (is.fail())
			return; // Not yet created.
		boost::archive::binary_iarchive ia{is};
		ia >> *this;
	}

	void Database::save()
	{
		if (!path.size())
			return;

		// TODO write to a temp file, and then mv temp path.
		std::ofstream os{path, std::ofstream::out | std::ofstream::binary};
		boost::archive::binary_oarchive oa{os};
		oa << *this;
	}

	auto Database::query(const sparqlxx::SPARQL& q) -> sparqlxx::Result
	{
		if (q.is<sparqlxx::Select>())
			return _query(q.get<sparqlxx::Select>());

		if (q.is<sparqlxx::Ask>())
			return _query(q.get<sparqlxx::Ask>());

		if (q.is<sparqlxx::Describe>())
			return _query(q.get<sparqlxx::Describe>());

		if (q.is<sparqlxx::InsertData>())
		{
			_update(q.get<sparqlxx::InsertData>());
			save();
		}
		else if (q.is<sparqlxx::DeleteData>())
		{
			_update(q.get<sparqlxx::DeleteData>());
			save();
		}

		return sparqlxx::None{};
	}
}

auto sparqlite_init_db(const char* dsn) -> sparqlxx::DatabaseImpl*
{
	return new sparqlite::Database(dsn);
}
