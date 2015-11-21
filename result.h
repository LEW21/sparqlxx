#pragma once

#include "triples.h"

namespace sparqlxx
{
	struct [[gnu::visibility("default")]] Solutions
	{
		std::vector<Var> vars;
		std::vector<std::vector<Term>> rows;

		Solutions() {}
		Solutions(std::vector<Var> vars): vars(std::move(vars)) {}
		Solutions(std::vector<Var> vars, std::vector<std::vector<Term>> rows): vars(vars), rows(rows) {}
	};

	struct [[gnu::visibility("default")]] None { None() {} };

	using Result = xx::variant<Solutions, bool, Triples, None>;

	template <typename T> struct result_of_t { using type = None; };
	template <> struct result_of_t<Select> { using type = Solutions; };
	template <> struct result_of_t<Ask> { using type = bool; };
	template <> struct result_of_t<Construct> { using type = Triples; };
	template <> struct result_of_t<Describe> { using type = Triples; };

	template <typename T>
	using result_of = typename result_of_t<T>::type;
}
