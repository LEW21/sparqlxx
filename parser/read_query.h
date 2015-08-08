#pragma once

#include "read_flatquery.h"
#include "to_algebra.h"

namespace sparqlxx
{
	template<>
	inline auto token_stream::read<Select>() -> Select
	{
		auto q = read<FlatSelect>();
		q.query.op = to_algebra(std::move(q.op));
		return q.query;
	}

	template<>
	inline auto token_stream::read<Construct>() -> Construct
	{
		auto q = read<FlatConstruct>();
		q.query.op = to_algebra(std::move(q.op));
		return q.query;
	}

	template<>
	inline auto token_stream::read<Describe>() -> Describe
	{
		auto q = read<FlatDescribe>();
		q.query.op = to_algebra(std::move(q.op));
		return q.query;
	}

	template<>
	inline auto token_stream::read<Ask>() -> Ask
	{
		auto q = read<FlatAsk>();
		q.query.op = to_algebra(std::move(q.op));
		return q.query;
	}

	template<>
	inline auto token_stream::read<Query>() -> Query
	{
		if (is("SELECT"))
			return read<Select>();
		if (is("CONSTRUCT"))
			return read<Construct>();
		if (is("DESCRIBE"))
			return read<Describe>();
		return read<Ask>();
	}
}
