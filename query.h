#pragma once

#include "atoms.h"
#include "algebra.h"
#include "dataset.h"
#include "triples.h"

namespace sparqlxx
{
	struct [[gnu::visibility("default")]] Select
	{
		Dataset dataset; // optional
		Algebra::AnyOp op;

		Select() {}

		Select(Dataset dataset, Algebra::AnyOp op): dataset(std::move(dataset)), op(std::move(op)) {}
		Select(Algebra::AnyOp op): op(std::move(op)) {}
	};

	struct [[gnu::visibility("default")]] Construct
	{
		Dataset dataset; // optional
		Algebra::AnyOp op;
		TriplesTemplateN tpl;

		Construct() {}
	};

	struct [[gnu::visibility("default")]] Describe
	{
		Dataset dataset; // optional
		Algebra::AnyOp op; // optional if terms provided and they don't contain Var
		vector<VarOrTerm> terms; // optional if op provided

		Describe() {}
	};

	struct [[gnu::visibility("default")]] Ask
	{
		Dataset dataset; // optional
		Algebra::AnyOp op;

		Ask() {}

		Ask(Dataset dataset, Algebra::AnyOp op): dataset(std::move(dataset)), op(std::move(op)) {}
		Ask(Algebra::AnyOp op): op(std::move(op)) {}
	};

	using Query = xx::variant<Select, Construct, Describe, Ask>;
}
