#include "think.h"

#include "database.h"

namespace sparqlite
{
	using namespace R;

	template <typename T>
	struct WithID
	{
		ID id;
		T s;

		WithID(ID id, T s): id(id), s(std::move(s)) {}
	};

	using Statement = WithID<Quad>;

	auto find(const Database& db, const Quad& pattern) -> std::vector<Statement>
	{
		std::vector<Statement> out;
		for (auto qID : db.stmt.match(pattern))
			out.emplace_back(qID, db.res.toLiteQuad(qID));
		return out;
	}

	auto think(const Database& db, ID nID, Quad nS) -> std::vector<InferredTriple>
	{
		auto n = Statement{nID, std::move(nS)};
		auto R = std::vector<InferredTriple>{}; // reasoned statements

		auto find = [&db, nS](ID s, ID p, ID o){return sparqlite::find(db, Quad{s, p, o, nS.graph});};
		auto infer = [&R](Triple t, std::vector<ID> inferredFrom){R.emplace_back(std::move(t), std::move(inferredFrom));};

		// Check for special things related to our subject.
		for (auto S_sameAs : find(n.s.subject, sameAs, 0))
			infer({S_sameAs.s.object, n.s.predicate, n.s.object}, {n.id, S_sameAs.id});

		// Check for special things related to our predicate.
		for (auto P_sameAs : find(n.s.predicate, sameAs, 0))
			infer({n.s.subject, P_sameAs.s.object, n.s.object}, {n.id, P_sameAs.id});
		for (auto P_equivalentProperty : find(n.s.predicate, equivalentProperty, 0))
			infer({n.s.subject, P_equivalentProperty.s.object, n.s.object}, {n.id, P_equivalentProperty.id});
		for (auto P_subPropertyOf : find(n.s.predicate, subPropertyOf, 0))
			infer({n.s.subject, P_subPropertyOf.s.object, n.s.object}, {n.id, P_subPropertyOf.id});

		for (auto P_inverseOf : find(n.s.predicate, inverseOf, 0))
			infer({n.s.object, P_inverseOf.s.object, n.s.subject}, {n.id, P_inverseOf.id});

		for (auto P_transitive : find(n.s.predicate, type, TransitiveProperty))
		{
			// We need A-Y-C for every A-Y-B B-Y-C pair.
			// If we are A-Y-B:
			for (auto s : find(n.s.object, n.s.predicate, 0))
				infer({n.s.subject, n.s.predicate, s.s.object}, {n.id, P_transitive.id, s.id});
			// If we are B-Y-C:
			for (auto s : find(0, n.s.predicate, n.s.subject))
				infer({s.s.subject, n.s.predicate, n.s.object}, {n.id, P_transitive.id, s.id});
		}

		for (auto P_symmetric : find(n.s.predicate, type, SymmetricProperty))
			infer({n.s.object, n.s.predicate, n.s.subject}, {n.id, P_symmetric.id});

		// Check for special things related to our object.
		for (auto O_sameAs : find(n.s.object, sameAs, 0))
			infer({n.s.subject, n.s.predicate, O_sameAs.s.object}, {n.id, O_sameAs.id});

		// OKAY, and now the other way.

		if (n.s.predicate == type)
		{
			if (n.s.object == TransitiveProperty)
			{
				// OK, so this is a transitive property!
				// We need to create A-Y-C for every s(A-Y-B) z(B-Y-C) pair.
				for (auto s : find(0, n.s.subject, 0))
					for (auto z : find(s.s.object, n.s.subject, 0))
						infer({s.s.subject, n.s.subject, z.s.object}, {n.id, s.id, z.id});
			}
			else if (n.s.object == SymmetricProperty)
			{
				// OK, so this is a symmetric property!
				// We need to copy all the statements using it in a reversed way.
				for (auto s : find(0, n.s.subject, 0))
					infer({s.s.object, n.s.subject, s.s.subject}, {n.id, s.id});
			}
		}
		else if (n.s.predicate == sameAs)
		{
			// Is currently added property sameAs? If so, then copy everything!
			// There is no difference in which way will we copy it - everything will be copied in the second way by reversed sameAs created before by the effect of SymmetricProperty.
			for (auto s : find(n.s.subject, 0, 0))
				infer({n.s.object, s.s.predicate, s.s.object}, {n.id, s.id});

			for (auto s : find(0, n.s.subject, 0))
				infer({s.s.subject, n.s.object, s.s.object}, {n.id, s.id});

			for (auto s : find(0, 0, n.s.subject))
				infer({s.s.subject, s.s.predicate, n.s.object}, {n.id, s.id});
		}
		else if (n.s.predicate == equivalentProperty || n.s.predicate == subPropertyOf)
		{
			for (auto s : find(0, n.s.subject, 0))
				infer({s.s.subject, n.s.object, s.s.object}, {n.id, s.id});
		}
		else if (n.s.predicate == inverseOf)
		{
			for (auto s : find(0, n.s.subject, 0))
				infer({s.s.object, n.s.object, s.s.subject}, {n.id, s.id});
		}
		// TODO equivalentClass, subClassOf

		// Remove useless ideas, like A => A.
		auto Rbak = std::move(R);
		R = {};
		for (auto r : Rbak)
		{
			bool recursive = false;
			for (auto rea : r.inferredFrom)
			{
				auto reason = db.res.toLiteQuad(rea);
				if (r.s.subject == reason.subject && r.s.predicate == reason.predicate && r.s.object == reason.object)
					recursive = true;
			}
			if (!recursive)
				R.emplace_back(r);
		}

		return R;
	}
}
