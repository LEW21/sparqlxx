#pragma once

#include "sparql.h"
#include "databaseimpl.h"
#include "parse.h"

#include <dlfcn.h>
#include <stdexcept>

#include <iostream>
#include <boost/algorithm/string/replace.hpp>

namespace sparqlxx
{
	using InitDB = auto (*)(const char*) -> DatabaseImpl*;

	struct DSN
	{
		std::string type;
		std::string iri;

		DSN(std::string type, std::string iri): type(std::move(type)), iri(std::move(iri)) {}
	};

	auto parseDSN(const std::string& dsn) -> DSN
	{
		auto plus_pos = dsn.find_first_of("+");
		auto colon_pos = dsn.find_first_of(":");

		if (plus_pos != std::string::npos && plus_pos < colon_pos)
			return DSN{dsn.substr(0, plus_pos), dsn.substr(plus_pos + 1)};
		else if (colon_pos != std::string::npos)
			return DSN{"sparql-" + dsn.substr(0, colon_pos), dsn};
		else
			return DSN{dsn, ""};
	}

	auto loadDatabaseImpl(const DSN& dsn) -> std::unique_ptr<DatabaseImpl>
	{
		auto lib = dlopen(("lib" + dsn.type + ".so").c_str(), RTLD_NOW | RTLD_GLOBAL);

		if (!lib)
			throw std::runtime_error{dlerror()};

		auto underscored_type = dsn.type;
		boost::replace_all(underscored_type, "-", "_");

		auto init = dlsym(lib, (underscored_type + "_init_db").c_str());

		if (!init)
			throw std::runtime_error{dlerror()};

		auto init_f = InitDB(init);
		return std::unique_ptr<DatabaseImpl>{init_f(dsn.iri.c_str())};
	}

	class [[gnu::visibility("default")]] Database
	{
		std::unique_ptr<DatabaseImpl> _db;

	public:
		Database(std::unique_ptr<DatabaseImpl> db): _db{std::move(db)} {}

		Database(const DSN& dsn): _db{loadDatabaseImpl(dsn)} {}
		Database(const std::string& dsn): _db{loadDatabaseImpl(parseDSN(dsn))} {}

		auto query(const SPARQL& q) -> Result {return _db->query(q);}
		auto query(const std::string& sparql) -> Result {return query(parse(sparql));}

		void insert(Quads q) { query(InsertData{std::move(q)}); }
		void remove(Quads q) { query(DeleteData{std::move(q)}); }

		auto select(Algebra::AnyOp op) -> Solution { return query(Select{std::move(op)}).get<Solution>(); }
		auto ask(Algebra::AnyOp op) -> bool { return query(Ask{std::move(op)}).get<bool>(); }

		auto select(QuadsVP q) -> Solution { return select(Algebra::make<Algebra::Quad>(std::move(q))); }
		auto ask(QuadsVP q) -> bool { return ask(Algebra::make<Algebra::Quad>(std::move(q))); }
	};
}
