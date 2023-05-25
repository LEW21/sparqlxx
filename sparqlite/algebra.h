#pragma once

#include <map>
#include <algorithm>
#include "../result.h"

namespace sparqlite
{
	namespace Algebra
	{
		using sparqlxx::Solutions;
		using sparqlxx::Var;
		using sparqlxx::Term;

		struct Break {};

		struct MapVars
		{
			struct MapVar
			{
				enum class Op { Ignore, Append, Check } op;
				int row_pos;

				MapVar(Op op, int row_pos): op(op), row_pos(row_pos) {}
			};

			std::vector<MapVar> map;
			std::unordered_map<std::string, int> dedup;
			std::vector<Var> vars;
			std::vector<Term> nr;
			int i = 0;

			void add_var(const Var& v)
			{
				if (dedup.count(v.name))
				{
					map.emplace_back(MapVar::Op::Check, dedup[v.name]);
				}
				else
				{
					dedup[v.name] = vars.size();
					map.emplace_back(MapVar::Op::Append, -1);
					vars.emplace_back(std::move(v));
				}
			}

			template <typename VT>
			void add_var(const VT& vt)
			{
				if (!vt.template is<Var>())
				{
					map.emplace_back(MapVar::Op::Ignore, -1);
					return;
				}

				add_var(vt.template get<Var>());
			}

			void add_term(const Term& t)
			{
				auto mv = map[i];
				switch (mv.op)
				{
					case MapVar::Op::Ignore:
						break;

					case MapVar::Op::Append:
						nr.emplace_back(t);
						break;

					case MapVar::Op::Check:
						if (nr[mv.row_pos] != t)
							throw Break{};
						break;
				}
				++i;
			}

			void reset()
			{
				nr = {};
				nr.reserve(vars.size());
				i = 0;
			}

			auto take_row() -> std::vector<Term>
			{
				assert(nr.size() == vars.size());
				return std::move(nr);
			}
		};

		inline auto join(Solutions&& s1, Solutions&& s2) -> Solutions
		{
			auto S = Solutions();

			auto mv = MapVars{};
			for (const auto& v : s1.vars)
				mv.add_var(v);
			for (const auto& v : s2.vars)
				mv.add_var(v);
			S.vars = mv.vars;

			for (const auto& r1 : s1.rows)
			{
				for (const auto& r2 : s2.rows)
				{
					try
					{
						for (const auto& v : r1)
							mv.add_term(v);
						for (const auto& v : r2)
							mv.add_term(v);
						S.rows.emplace_back(mv.take_row());
					}
					catch (Break&) {}
					mv.reset();
				}
			}

			return S;
		}

		inline auto join(std::vector<Solutions>&& solutions) -> Solutions
		{
			// TODO optimize order

			auto S = std::move(solutions[0]);

			for (int i = 1; i < solutions.size(); ++i)
				S = join(std::move(S), std::move(solutions[i]));

			return S;
		}

		inline auto leftjoin(Solutions&& s1, Solutions&&) -> Solutions
		{
			return s1; // TODO
		}

		inline auto minus(Solutions&& s1, Solutions&&) -> Solutions
		{
			return s1; // TODO
		}

		inline auto union_(Solutions&& s1, Solutions&&) -> Solutions
		{
			return s1; // TODO
		}

		inline auto extend(Solutions&& s, const Var&, const sparqlxx::Expression&) -> Solutions
		{
			return s; // TODO
		}

		inline auto reduced(Solutions&& s) -> Solutions
		{
			auto last = std::unique(s.rows.begin(), s.rows.end());
			s.rows.erase(last, s.rows.end());
			return s;
		}

		inline auto distinct(Solutions&& s) -> Solutions
		{
			return reduced(std::move(s)); // TODO
		}

		inline auto project(Solutions&& s, const std::vector<Var>& vars) -> Solutions
		{
			auto provided_vars = std::map<std::string, int>{};

			auto i = 0;
			for (auto& v : s.vars)
				provided_vars[std::move(v.name)] = i++;

			auto map_vars = std::vector<int>{};

			for (const auto& v : vars)
				try
				{
					map_vars.emplace_back(provided_vars.at(v.name));
				}
				catch (std::out_of_range&)
				{
					throw std::invalid_argument("Variable '" + v.name + "' is used in the query result set but not assigned.");
				}

			auto S = Solutions{vars};
			S.rows.reserve(s.rows.size());

			for (auto& r : s.rows)
			{
				S.rows.emplace_back();
				auto& R = S.rows.back();
				R.reserve(vars.size());

				for (auto i = 0; i < vars.size(); ++i)
					R.emplace_back(r[map_vars[i]]);
			}

			return S;
		}

		inline auto slice(Solutions&& s, int start, std::optional<int> length) -> Solutions
		{
			auto b = s.rows.begin() + start;
			auto e = length ? (start + *length < s.rows.size() ? b + *length : s.rows.end()) : s.rows.end();
			return Solutions{std::move(s.vars), {b, e}};
		}
	}
}
