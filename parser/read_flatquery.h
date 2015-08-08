#pragma once

#include "../query.h"
#include "flatqueryop.h"
#include "token_stream.h"

namespace sparqlxx
{
	template <typename T>
	struct FlatQuery
	{
		T query;
		FlatQueryOp op;
	};

	using FlatSelect = FlatQuery<Select>;
	using FlatConstruct = FlatQuery<Construct>;
	using FlatDescribe = FlatQuery<Describe>;
	using FlatAsk = FlatQuery<Ask>;

	template<>
	auto token_stream::read<FlatSelect>() -> FlatSelect;

	template<>
	auto token_stream::read<FlatConstruct>() -> FlatConstruct;

	template<>
	auto token_stream::read<FlatDescribe>() -> FlatDescribe;

	template<>
	auto token_stream::read<FlatAsk>() -> FlatAsk;
}
