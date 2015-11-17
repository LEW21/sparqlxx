#pragma once

#include "atoms.h"
#include "algebra.h"
#include "dataset.h"
#include "triples.h"

namespace sparqlxx
{
	// SPARQL Update operations - inline namespace
	inline namespace UpdateCmd
	{
		using GraphRef = Iri;

		struct [[gnu::visibility("default")]] Default {Default(){} Default(const Default&){}};
		struct [[gnu::visibility("default")]] Named {Named(){} Named(const Named&){}};
		struct [[gnu::visibility("default")]] All {All(){} All(const All&){}};
		using GraphRefAll = xx::variant<GraphRef, Default, Named, All>;

		using GraphOrDefault = xx::variant<GraphRef, Default>;

		struct [[gnu::visibility("default")]] Create
		{
			bool silent;
			GraphRef what;

			Create(bool silent, GraphRef&& what): silent(silent), what(std::move(what)) {}
		};

		struct [[gnu::visibility("default")]] Load
		{
			bool silent;
			Iri what;
			optional<GraphRef> into;

			Load(bool silent, Iri&& what, optional<GraphRef>&& into = nullopt): silent(silent), what(std::move(what)), into(std::move(into)) {}
		};

		struct [[gnu::visibility("default")]] Clear
		{
			bool silent;
			GraphRefAll what;

			Clear(bool silent, GraphRefAll&& what): silent(silent), what(std::move(what)) {}
		};

		struct [[gnu::visibility("default")]] Drop
		{
			bool silent;
			GraphRefAll what;

			Drop(bool silent, GraphRefAll&& what): silent(silent), what(std::move(what)) {}
		};

		struct [[gnu::visibility("default")]] MassOp
		{
			enum class Type { Move, Copy, Add } type;
			bool silent;
			GraphOrDefault from;
			GraphOrDefault to;

			MassOp(Type type, bool silent, GraphOrDefault&& from, GraphOrDefault&& to): type(type), silent(silent), from{std::move(from)}, to{std::move(to)} {}
		};

		struct [[gnu::visibility("default")]] InsertData
		{
			Quads data;

			InsertData(Quads&& data): data(std::move(data)) {}
		};

		struct [[gnu::visibility("default")]] DeleteData
		{
			Quads data;

			DeleteData(Quads&& data): data(std::move(data)) {}
		};

		struct [[gnu::visibility("default")]] Modify
		{
			optional<Iri> with;
			QuadPatternN delete_;
			QuadPatternN insert_;
			Dataset using_;
			Algebra::AnyOp where;

			Modify() {}
		};

		inline auto DeleteWhere(QuadPatternN&& data)
		{
			auto q = Modify{};
			q.delete_ = data;
			q.where = Algebra::make<Algebra::Quad>(std::move(data));
			return q;
		}

		using ModifyOrData = xx::variant<InsertData, DeleteData, Modify>;
	}

	using Update = xx::variant<InsertData, DeleteData, Modify, MassOp, Create, Load, Clear, Drop>;
}
