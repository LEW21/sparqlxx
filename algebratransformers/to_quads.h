#pragma once

#include "../algebratransformer.h"

namespace sparqlxx
{
	namespace AlgebraTransformers
	{
		struct ToQuads : public AlgebraTransformer
		{
			VarOrIri graph;

			ToQuads(const VarOrIri& graph): graph{graph} {}
			ToQuads(VarOrIri&& graph): graph{std::move(graph)} {}

			using AlgebraTransformer::transform;

			auto transform(Algebra::Graph&& g) -> Algebra::Op override
			{
				return ToQuads(std::move(g.graph))(std::move(*g.data));
			}

			auto transform(Algebra::Basic&& b) -> Algebra::Op override
			{
				auto q = Algebra::Quad{};
				for (auto& p : b.triples)
					q.quads.emplace_back(std::move(p.subject), std::move(p.predicate), std::move(p.object), graph);
				return q;
			}
		};

		auto to_quads(Algebra::Op&& op, const VarOrIri& default_graph) -> Algebra::Op
		{
			return ToQuads(default_graph)(std::move(op));
		}

		auto to_quads(Algebra::Op&& op, VarOrIri&& default_graph) -> Algebra::Op
		{
			return ToQuads(std::move(default_graph))(std::move(op));
		}
	}
}
