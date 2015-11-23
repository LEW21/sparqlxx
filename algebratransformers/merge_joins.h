#pragma once

#include "../algebratransformer.h"

namespace sparqlxx
{
	namespace AlgebraTransformers
	{
		struct MergeJoins : public AlgebraTransformer
		{
			using AlgebraTransformer::transform;

			auto transform(Algebra::Join&& op) -> Algebra::Op override
			{
				auto new_ops = std::vector<Algebra::AnyOp>{};
				std::function<void(Algebra::Join&&)> handle_join = [&new_ops, &handle_join](Algebra::Join&& op) -> void
				{
					for (auto& subop : op.ops)
					{
						if (subop->is<Algebra::Join>())
							handle_join(std::move(subop->get<Algebra::Join>()));
						else if (subop->is<Algebra::Null>())
							{}
						else
							new_ops.emplace_back(subop);
					}
				};
				handle_join(std::move(op));
				op.ops = new_ops;

				if (op.ops.size() == 0)
					return Algebra::Null{};
				else if (op.ops.size() == 1)
					return *op.ops[0];
				else
					return op;
			}
		};

		auto merge_joins(Algebra::Op&& op) -> Algebra::Op
		{
			return MergeJoins()(std::move(op));
		}
	}
}
