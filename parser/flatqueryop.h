#pragma once

#include <boost/optional.hpp>
#include <vector>
#include "../atoms.h"
#include "../triples.h"
#include "ggp.h"

namespace sparqlxx
{
	struct Binding
	{
		Expression from;
		Var to;

		Binding(const Expression& e, const Var& v): from{e}, to{v} {}
		Binding(const Var& v): from{v}, to{v} {}

		Binding(Expression&& e, Var&& v): from{std::move(e)}, to{std::move(v)} {}
		Binding(Var&& v): from{v}, to{std::move(v)} {}
	};

	struct FlatQueryOp
	{
		// SELECT
		bool distinct = false;
		bool reduced = false;

		// SELECT, DESCRIBE
		optional<vector<Binding>> projection;

		// everywhere
		GroupGraphPattern where; // optional in DESCRIBE, required in others.
		std::vector<Constraint> having;
		optional<int> limit;
		optional<int> offset;
		//optional<???> values;
	};
}
