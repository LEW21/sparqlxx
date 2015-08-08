#include "database.h"

#include "think.h"

namespace sparqlite
{
	inline auto insert(Database& db, const Quad& s, bool user = true) -> ID
	{
		auto id = db.res[s];
		auto metadata = std::vector<Quad>{};

		auto meta = [&](ID S, ID P, ID O){
			auto q = Quad{S, P, O, s.graph};
			db.stmt.insert(db.res[q], q);
		};

		if (user)
		{
			auto source = db.res.blank();
			meta(id, R::source, source);
			meta(source, R::type, R::Insertion);
		}

		if (!db.stmt.match(s).size()) // Already exists?
		{
			db.stmt.insert(id, s);

			//meta(id, R::type, R::Statement);
			//meta(id, R::subject, s.subject);
			//meta(id, R::predicate, s.predicate);
			//meta(id, R::object, s.object);

			auto inferred = think(db, id, s);

			for (auto& inf : inferred)
			{
				auto infQ = Quad{inf.s.subject, inf.s.predicate, inf.s.object, s.graph};
				auto infId = db.res[infQ];

				auto source = db.res.blank();
				meta(infId, R::source, source);
				meta(source, R::type, R::Inference);

				for (auto iFrom : inf.inferredFrom)
					meta(source, R::inferredFrom, iFrom);

				insert(db, infQ, false);
			}
		}

		return id;
	}

	template <>
	auto Database::_query<sparqlxx::InsertData>(const sparqlxx::InsertData& id) -> sparqlxx::None
	{
		auto blanks = std::unordered_map<std::string, ID>{};

		auto conv_term = [&](const sparqlxx::Term& v)
		{
			if (v.is<sparqlxx::BlankNode>())
			{
				auto b = v.get<sparqlxx::BlankNode>().name;
				try
				{
					return blanks.at(b);
				}
				catch (std::out_of_range&)
				{
					return blanks[b] = res.blank();
				}
			}
			else
				return res[v];
		};

		for (const auto& sq : id.data)
		{
			auto q = Quad{conv_term(sq.subject), res[sq.predicate], conv_term(sq.object), res[sq.graph]};
			insert(*this, q);
		}

		return {};
	}

	template <>
	auto Database::_query<sparqlxx::DeleteData>(const sparqlxx::DeleteData& dd) -> sparqlxx::None
	{
		for (const auto& sq : dd.data)
		{
			auto q = Quad{res[sq.subject], res[sq.predicate], res[sq.object], res[sq.graph]};
			stmt.remove(res[q], q);
		}

		return {};
	}
}
