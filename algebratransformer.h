#pragma once

#include "algebra.h"

namespace sparqlxx
{
	struct AlgebraTransformer
	{
		virtual auto operator()(Algebra::Op&& o) -> Algebra::Op;

		virtual auto transform(Algebra::Null&& n) -> Algebra::Op
		{
			return n;
		}

		virtual auto transform(Algebra::Basic&& b) -> Algebra::Op
		{
			return b;
		}

		virtual auto transform(Algebra::Quad&& q) -> Algebra::Op
		{
			return q;
		}

		virtual auto transform(Algebra::Join&& op) -> Algebra::Op
		{
			for (auto& subop : op.ops)
				*subop = (*this)(std::move(*subop));
			return op;
		}

		virtual auto transform(Algebra::LeftJoin&& op) -> Algebra::Op
		{
			*op.a = (*this)(std::move(*op.a));
			*op.b = (*this)(std::move(*op.b));
			return op;
		}

		virtual auto transform(Algebra::Minus&& op) -> Algebra::Op
		{
			*op.a = (*this)(std::move(*op.a));
			*op.b = (*this)(std::move(*op.b));
			return op;
		}

		virtual auto transform(Algebra::Union&& op) -> Algebra::Op
		{
			*op.a = (*this)(std::move(*op.a));
			*op.b = (*this)(std::move(*op.b));
			return op;
		}

		virtual auto transform(Algebra::Graph&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}

		virtual auto transform(Algebra::Service&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}

		virtual auto transform(Algebra::Filter&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}

		virtual auto transform(Algebra::Extend&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}

		virtual auto transform(Algebra::Project&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}

		virtual auto transform(Algebra::Distinct&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}

		virtual auto transform(Algebra::Reduced&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}

		virtual auto transform(Algebra::Slice&& op) -> Algebra::Op
		{
			*op.data = (*this)(std::move(*op.data));
			return op;
		}
	};

	inline auto AlgebraTransformer::operator()(Algebra::Op&& o) -> Algebra::Op
	{
		return o.call([this](auto& op){return transform(std::move(op));});
	}
}
