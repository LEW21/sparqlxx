#pragma once

#include "../algebratransformer.h"

namespace sparqlxx
{
	namespace AlgebraTransformers
	{
		struct MergeBasics : public AlgebraTransformer
		{
			using AlgebraTransformer::transform;

			auto transform(Algebra::Join&& op) -> Algebra::Op override
			{
				auto triples = TriplesVP{};
				auto quads = QuadsVP{};
				auto other_ops = std::vector<Algebra::AnyOp>{};

				for (auto& subop : op.ops)
				{
					if (subop->is<Algebra::Basic>())
					{
						auto& new_triples = subop->get<Algebra::Basic>().triples;
						triples.reserve(triples.size() + new_triples.size());
						triples.insert(triples.end(), std::make_move_iterator(new_triples.begin()), std::make_move_iterator(new_triples.end()));
					}
					else if (subop->is<Algebra::Quad>())
					{
						auto& new_quads = subop->get<Algebra::Quad>().quads;
						quads.reserve(quads.size() + new_quads.size());
						quads.insert(quads.end(), std::make_move_iterator(new_quads.begin()), std::make_move_iterator(new_quads.end()));
					}
					else
						other_ops.emplace_back(std::move(subop));
				}

				op.ops = {};
				op.ops.reserve(other_ops.size() + (triples.size()?1:0) + (quads.size()?1:0));
				if (triples.size())
					op.ops.emplace_back(Algebra::make<Algebra::Basic>(std::move(triples)));
				if (quads.size())
					op.ops.emplace_back(Algebra::make<Algebra::Quad>(std::move(quads)));
				for (auto& other : other_ops)
					op.ops.emplace_back(std::move(other));

				if (op.ops.size() == 1)
					return *op.ops[0];
				else
					return op;
			}
		};

		auto merge_basics(Algebra::Op&& op) -> Algebra::Op
		{
			return MergeBasics()(std::move(op));
		}
	}
}
