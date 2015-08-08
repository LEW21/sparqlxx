#pragma once

#include <vector>
#include "atoms.h"
#include "triples.h"
#include "copy_ptr.h"

namespace sparqlxx
{
	namespace Algebra
	{
		struct Null;
		struct Basic;
		struct Quad;
		struct Join;
		struct LeftJoin;
		struct Minus;
		struct Union;
		struct Graph;
		struct Service;
		struct Filter;
		struct Extend;
		struct Project;
		struct Distinct;
		struct Reduced;
		struct Slice;

		using Op = xx::variant<Null, Basic, Quad, Join, LeftJoin, Minus, Union, Graph, Service, Filter, Extend, Project, Distinct, Reduced, Slice>;
		using AnyOp = copy_ptr<Op>;

		struct [[gnu::visibility("default")]] Null {Null(){} Null(const Null&){}};

		struct [[gnu::visibility("default")]] Basic
		{
			TriplesVP triples;
			Basic() {}
			Basic(const TriplesVP& triples): triples(triples) {}
			Basic(TriplesVP&& triples): triples(std::move(triples)) {}
		};

		struct [[gnu::visibility("default")]] Quad
		{
			QuadsVP quads;
			Quad() {}
			Quad(const QuadsVP& quads): quads(quads) {}
			Quad(QuadsVP&& quads): quads(std::move(quads)) {}
		};

		struct [[gnu::visibility("default")]] Join
		{
			AnyOp a, b;
			Join(AnyOp a, AnyOp b): a(std::move(a)), b(std::move(b)) {}
		};

		struct [[gnu::visibility("default")]] LeftJoin
		{
			AnyOp a, b;
			LeftJoin(AnyOp a, AnyOp b): a(std::move(a)), b(std::move(b)) {}
		};

		struct [[gnu::visibility("default")]] Minus
		{
			AnyOp a, b;
			Minus(AnyOp a, AnyOp b): a(std::move(a)), b(std::move(b)) {}
		};

		struct [[gnu::visibility("default")]] Union
		{
			AnyOp a, b;
			Union(AnyOp a, AnyOp b): a(std::move(a)), b(std::move(b)) {}
		};

		struct [[gnu::visibility("default")]] Graph
		{
			VarOrIri graph;
			AnyOp data;
			Graph(VarOrIri graph, AnyOp data): graph(std::move(graph)), data(std::move(data)) {}
		};

		struct [[gnu::visibility("default")]] Service
		{
			bool silent;
			VarOrIri iri;
			AnyOp data;
			Service(bool silent, VarOrIri iri, AnyOp data): silent(silent), iri(std::move(iri)), data(std::move(data)) {}
		};

		struct [[gnu::visibility("default")]] Filter
		{
			vector<Constraint> constraints;
			AnyOp data;
			Filter(vector<Constraint> constraints, AnyOp data): constraints(std::move(constraints)), data(std::move(data)) {}
		};

		struct [[gnu::visibility("default")]] Extend
		{
			AnyOp data;
			Var var;
			Expression exp;
			Extend(AnyOp data, Var var, Expression exp): data(std::move(data)), var(std::move(var)), exp(std::move(exp)) {}
		};

		struct [[gnu::visibility("default")]] Project
		{
			AnyOp data;
			vector<Var> vars;
			Project(AnyOp data, vector<Var> vars): data(std::move(data)), vars(std::move(vars)) {}
		};

		struct [[gnu::visibility("default")]] Distinct
		{
			AnyOp data;
			Distinct(AnyOp data): data(std::move(data)) {}
		};

		struct [[gnu::visibility("default")]] Reduced
		{
			AnyOp data;
			Reduced(AnyOp data): data(std::move(data)) {}
		};

		struct [[gnu::visibility("default")]] Slice
		{
			AnyOp data;
			int start;
			optional<int> length;
			Slice(AnyOp data, optional<int> start, optional<int> length): data(std::move(data)), start(start ? *start : 0), length(length) {}
		};

		template <typename T, typename... A>
		inline auto make(A&&... arg) -> AnyOp
		{
			return std::make_unique<Op>(xx::in_place<T>, std::forward<A>(arg)...);
		}
	}
}
