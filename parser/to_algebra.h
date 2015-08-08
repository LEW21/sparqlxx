#pragma once

#include "flatqueryop.h"
#include "../algebra.h"

namespace sparqlxx
{
	inline auto to_algebra(FlatQueryOp&& q) -> Algebra::AnyOp
	{
		using namespace Algebra;

		if (!q.where)
			return {};

		auto X = std::move(q.where);

		// 18.2.4.1 Grouping and Aggregation

		// 18.2.4.2 HAVING
		if (q.having.size())
		{
			if (X->is<Algebra::Filter>())
			{
				auto& f = X->get<Algebra::Filter>().constraints;
				f.insert(f.end(), q.having.begin(), q.having.end());
			}
			else
				X = make<Filter>(std::move(q.having), std::move(X));
		}

		// 18.2.4.3 VALUES

		// 18.2.4.4 SELECT Expressions
		if (q.projection)
			for (const auto& b : *q.projection)
			{
				if (!b.from.is<Var>() || b.from.get<Var>() != b.to)
					X = make<Extend>(std::move(X), b.to, b.from);
			}

		// 18.2.5.1 ORDER BY

		// 18.2.5.2 Projection
		if (q.projection)
		{
			auto project_into = std::vector<Var>{};
			for (const auto& b : *q.projection)
				project_into.emplace_back(b.to);

			X = make<Project>(std::move(X), std::move(project_into));
		}

		// 18.2.5.3 DISTINCT
		if (q.distinct)
			X = make<Distinct>(std::move(X));

		// 18.2.5.4 REDUCED
		if (q.reduced)
			X = make<Reduced>(std::move(X));

		// 18.2.5.5 OFFSET and LIMIT
		if (q.offset || q.limit)
			X = make<Slice>(std::move(X), q.offset, q.limit);

		// 18.2.5.6 Final Algebra Expression
		return X;
	}
}
