#pragma once

#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include "quad.h"

namespace sparqlite
{
	inline auto is_valid(const Quad& q)
	{
		return q.subject && q.predicate && q.object && q.graph;
	}

	template <typename... T>
	struct ImplicitTuple: public std::tuple<T...>
	{
		template <typename... U>
		constexpr ImplicitTuple(U&&... a): std::tuple<T...>(std::forward<U>(a)...) {}
	};

	using P1 = ID;
	using P2 = ImplicitTuple<ID, ID>;
	using P3 = ImplicitTuple<ID, ID, ID>;
	using P4 = ImplicitTuple<ID, ID, ID, ID>;
}

namespace std
{
	template<>
	struct hash<sparqlite::P2>
	{
		inline auto operator()(const sparqlite::P2& p) const
		{
			return get<0>(p) ^ get<1>(p);
		}
	};

	template<>
	struct hash<sparqlite::P3>
	{
		inline auto operator()(const sparqlite::P3& p) const
		{
			return get<0>(p) ^ get<1>(p) ^ get<2>(p);
		}
	};

	template<>
	struct hash<sparqlite::P4>
	{
		inline auto operator()(const sparqlite::P4& p) const
		{
			return get<0>(p) ^ get<1>(p) ^ get<2>(p) ^ get<3>(p);
		}
	};
}

namespace sparqlite
{
	struct Statements
	{
		std::unordered_set<ID> all;

		std::unordered_multimap<P1, ID> byS;
		std::unordered_multimap<P1, ID> byP;
		std::unordered_multimap<P1, ID> byO;
		std::unordered_multimap<P1, ID> byG;

		std::unordered_multimap<P2, ID> bySP;
		std::unordered_multimap<P2, ID> bySO;
		std::unordered_multimap<P2, ID> bySG;
		std::unordered_multimap<P2, ID> byPO;
		std::unordered_multimap<P2, ID> byPG;
		std::unordered_multimap<P2, ID> byOG;

		std::unordered_multimap<P3, ID> bySPO;
		std::unordered_multimap<P3, ID> bySPG;
		std::unordered_multimap<P3, ID> bySOG;
		std::unordered_multimap<P3, ID> byPOG;

		std::unordered_multimap<P4, ID> bySPOG;

		void insert(ID, const Quad&);
		void remove(ID, const Quad&);

		auto match(const Quad&) const -> std::unordered_set<ID>;
	};

	inline void Statements::insert(ID id, const Quad& s)
	{
		all.emplace(id);

		byS.emplace(s.subject, id);
		byP.emplace(s.predicate, id);
		byO.emplace(s.object, id);
		byG.emplace(s.graph, id);

		bySP.emplace(P2{s.subject, s.predicate}, id);
		bySO.emplace(P2{s.subject, s.object}, id);
		bySG.emplace(P2{s.subject, s.graph}, id);
		byPO.emplace(P2{s.predicate, s.object}, id);
		byPG.emplace(P2{s.predicate, s.graph}, id);
		byOG.emplace(P2{s.object, s.graph}, id);

		bySPO.emplace(P3{s.subject, s.predicate, s.object}, id);
		bySPG.emplace(P3{s.subject, s.predicate, s.graph}, id);
		bySOG.emplace(P3{s.subject, s.object, s.graph}, id);
		byPOG.emplace(P3{s.predicate, s.object, s.graph}, id);

		bySPOG.emplace(P4{s.subject, s.predicate, s.object, s.graph}, id);
	}

	template <typename P>
	inline void erase(std::unordered_multimap<P, ID>& map, P match, ID id)
	{
		auto look_in = map.equal_range(match);
		for (auto i = look_in.first; i != look_in.second; ++i)
			if (i->second == id)
			{
				map.erase(i);
				return;
			}
	}

	inline void Statements::remove(ID id, const Quad& s)
	{
		all.erase(id);

		erase(byS, s.subject, id);
		erase(byP, s.predicate, id);
		erase(byO, s.object, id);
		erase(byG, s.graph, id);

		erase(bySP, {s.subject, s.predicate}, id);
		erase(bySO, {s.subject, s.object}, id);
		erase(bySG, {s.subject, s.graph}, id);
		erase(byPO, {s.predicate, s.object}, id);
		erase(byPG, {s.predicate, s.graph}, id);
		erase(byOG, {s.object, s.graph}, id);

		erase(bySPO, {s.subject, s.predicate, s.object}, id);
		erase(bySPG, {s.subject, s.predicate, s.graph}, id);
		erase(bySOG, {s.subject, s.object, s.graph}, id);
		erase(byPOG, {s.predicate, s.object, s.graph}, id);

		erase(bySPOG, {s.subject, s.predicate, s.object, s.graph}, id);
	}

	template <typename R>
	inline auto V(R r)
	{
		auto s = std::unordered_set<ID>{};
		for (auto i = r.first; i != r.second; ++i)
			s.insert(i->second);
		return s;
	}

	#define FIND4(BY, A, B, C, D) return V(BY.equal_range({A, B, C, D}));
	#define FIND3(BY, A, B, C)    return V(BY.equal_range({A, B, C}));
	#define FIND2(BY, A, B)       return V(BY.equal_range({A, B}));
	#define FIND1(BY, A)          return V(BY.equal_range(A));

	inline auto Statements::match(const Quad& s) const -> std::unordered_set<ID>
	{
		auto S = s.subject, P = s.predicate, O = s.object, G = s.graph;

		if (S)
		{
			if (P)
			{
				if (O)
				{
					if (G)
						FIND4(bySPOG, S, P, O, G)
						else
							FIND3(bySPO, S, P, O);
				}
				else
				{
					if (G)
						FIND3(bySPG, S, P, G)
						else
							FIND2(bySP, S, P);
				}
			}
			else
			{
				if (O)
				{
					if (G)
						FIND3(bySOG, S, O, G)
						else
							FIND2(bySO, S, O);
				}
				else
				{
					if (G)
						FIND2(bySG, S, G)
						else
							FIND1(byS, S);
				}
			}
		}
		else
		{
			if (P)
			{
				if (O)
				{
					if (G)
						FIND3(byPOG, P, O, G)
						else
							FIND2(byPO, P, O);
				}
				else
				{
					if (G)
						FIND2(byPG, P, G)
						else
							FIND1(byP, P);
				}
			}
			else
			{
				if (O)
				{
					if (G)
						FIND2(byOG, O, G)
						else
							FIND1(byO, O);
				}
				else
				{
					if (G)
						FIND1(byG, G)
						else
							return all;
				}
			}
		}
	}
}
