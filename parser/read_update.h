#pragma once

#include "../update.h"
#include "read_dataset.h"
#include "read_quads.h"

namespace sparqlxx
{
	template<>
	inline auto token_stream::read<GraphRefAll>() -> GraphRefAll
	{
		if (try_match("GRAPH"))
			return read<Iri>();
		if (try_match("DEFAULT"))
			return Default{};
		if (try_match("NAMED"))
			return Named{};
		match("ALL");
		return All{};
	}

	template<>
	inline auto token_stream::read<GraphOrDefault>() -> GraphOrDefault
	{
		if (try_match("DEFAULT"))
			return Default{};
		try_match("GRAPH");
		return read<Iri>();
	}

	template<>
	inline auto token_stream::read<Load>() -> Load
	{
		match("LOAD");
		return Load{try_match("SILENT"), read<Iri>(), try_match("INTO") ? (match("GRAPH"), make_optional(read<Iri>())) : nullopt};
	}

	template<>
	inline auto token_stream::read<Clear>() -> Clear
	{
		match("CLEAR");
		return Clear{try_match("SILENT"), read<GraphRefAll>()};
	}

	template<>
	inline auto token_stream::read<Drop>() -> Drop
	{
		match("DROP");
		return Drop{try_match("SILENT"), read<GraphRefAll>()};
	}

	template<>
	inline auto token_stream::read<Create>() -> Create
	{
		match("CREATE");
		return Create{try_match("SILENT"), (match("GRAPH"), read<Iri>())};
	}

	template<>
	inline auto token_stream::read<MassOp>() -> MassOp
	{
		auto type =
			try_match("ADD")  ? MassOp::Type::Add :
			try_match("MOVE") ? MassOp::Type::Move :
			try_match("COPY") ? MassOp::Type::Copy :
			throw error();

		return MassOp{type, try_match("SILENT"), read<GraphOrDefault>(), (match("TO"), read<GraphOrDefault>())};
	}

	template<>
	inline auto token_stream::read<ModifyOrData>() -> ModifyOrData
	{
		Modify m;
		m.with = try_match("WITH") ? make_optional(read<Iri>()) : nullopt;

		auto had_delete_or_insert = false;

		if (try_match("DELETE"))
		{
			if (!m.with && try_match("DATA"))
			{
				match("{");
				auto data = read<QuadDataN>();
				match("}");
				return ModifyOrData{DeleteData{std::move(data)}};
			}

			if (!m.with && try_match("WHERE"))
			{
				match("{");
				auto pattern = read<QuadPatternN>();
				match("}");
				return DeleteWhere(std::move(pattern));
			}

			had_delete_or_insert = true;

			match("{");
			m.delete_ = read<QuadPatternN>();
			match("}");
		}

		if (try_match("INSERT"))
		{
			if (!m.with && !had_delete_or_insert && try_match("DATA"))
			{
				match("{");
				auto data = read<QuadDataN>();
				match("}");
				return InsertData{std::move(data)};
			}

			had_delete_or_insert = true;

			match("{");
			m.insert_ = read<QuadPatternN>();
			match("}");
		}

		if (!had_delete_or_insert)
			throw error();

		m.using_ = read_dataset(*this, "USING");

		match("WHERE");

		m.where = read<GroupGraphPattern>();
		return m;
	}

	template<>
	inline auto token_stream::read<Update>() -> Update
	{
		if (is("CREATE"))
			return read<Create>();
		if (is("LOAD"))
			return read<Load>();
		if (is("CLEAR"))
			return read<Clear>();
		if (is("DROP"))
			return read<Drop>();
		if (is("ADD") || is("MOVE") || is("COPY"))
			return read<MassOp>();
		return read<ModifyOrData>();
	}
}
