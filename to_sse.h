#include "algebra.h"

namespace sparqlxx
{
	namespace Algebra
	{
		/* Serialize the SPARQL algebra to SSE
		 *
		 * Do it in a way as similar to Apache Jena as possible
		 * without sacrificing sanity (= preserving qualified names in the algebra).
		 */
		//- Make the parameters explicitely-typed.
		template <typename F> auto to_sse(const F&, int level = 0) -> std::string;

		auto to_sse(const Op& p, int level = 0) -> std::string;

		inline auto to_sse(const AnyOp& p, int level = 0) -> std::string
		{
			return to_sse(*p, level);
		}

		inline auto to_sse(const Null&, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(table unit)\n";
		}

		inline auto to_sse(const Basic& b, int level = 0) -> std::string
		{
			if (b.triples.size() == 1)
				return std::string(level, '\t') + "(bgp [" + to_sparql(b.triples[0].subject) + " " + to_sparql(b.triples[0].predicate) + " " + to_sparql(b.triples[0].object) + "])\n";

			auto s = std::string(level, '\t') + std::string{"(bgp \n"};
			for (const auto& triple : b.triples)
				s += std::string(level+1, '\t') + "[" + to_sparql(triple.subject) + " " + to_sparql(triple.predicate) + " " + to_sparql(triple.object) + "]\n";
			s += std::string(level, '\t') + ")\n";
			return s;
		}

		inline auto to_sse(const Quad& q, int level = 0) -> std::string
		{
			if (q.quads.size() == 1)
				return std::string(level, '\t') + "(quadpattern [" + to_sparql(q.quads[0].graph) + " " + to_sparql(q.quads[0].subject) + " " + to_sparql(q.quads[0].predicate) + " " + to_sparql(q.quads[0].object) + "])\n";

			auto s = std::string(level, '\t') + "(quadpattern \n";
			for (const auto& quad : q.quads)
				s += std::string(level+1, '\t') + "[" + to_sparql(quad.graph) + " " + to_sparql(quad.subject) + " " + to_sparql(quad.predicate) + " " + to_sparql(quad.object) + "]\n";
			s += std::string(level, '\t') + ")\n";
			return s;
		}

		inline auto to_sse(const Join& j, int level = 0) -> std::string
		{
			auto s = std::string(level, '\t') + "(join\n";
			for (const auto& subop : j.ops)
				s += to_sse(subop, level+1);
			s += std::string(level, '\t') + ")\n";
			return s;
		}

		inline auto to_sse(const LeftJoin& j, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(leftjoin\n" + to_sse(j.a, level+1) + to_sse(j.b, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Minus& j, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(minus\n" + to_sse(j.a, level+1) + to_sse(j.b, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Union& j, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(union\n" + to_sse(j.a, level+1) + to_sse(j.b, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Graph& g, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(graph " + to_sparql(g.graph) + "\n" + to_sse(g.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Service& s, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(service " + (s.silent ? "silent " : "") + to_sparql(s.iri) + "\n" + to_sse(s.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Filter& f, int level = 0) -> std::string
		{
			if (f.constraints.size() == 1)
				return std::string(level, '\t') + "(filter " + to_sparql(f.constraints[0]) + "\n" + to_sse(f.data, level+1) + std::string(level, '\t') + ")\n";

			auto s = std::string(level, '\t') + "(filter (exprlist";
			for (const auto& c : f.constraints)
				s += " " + to_sparql(c);
			return s + ")\n" + to_sse(f.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Extend& e, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(extend ((" + to_sparql(e.var) + " " + to_sparql(e.exp) + "))\n" + to_sse(e.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Project& p, int level = 0) -> std::string
		{
			auto s = std::string(level, '\t') + "(project (";
			bool first = true;
			for (const auto& v : p.vars)
			{
				if (first)
					first = false;
				else
					s += " ";
				s += to_sparql(v);
			}
			return s + ")\n" + to_sse(p.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Distinct& o, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(distinct\n" + to_sse(o.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Reduced& o, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(reduce\n" + to_sse(o.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Slice& o, int level = 0) -> std::string
		{
			return std::string(level, '\t') + "(slice " + std::to_string(o.start) + " " + (o.length ? std::to_string(*o.length) : "inf") + "\n" + to_sse(o.data, level+1) + std::string(level, '\t') + ")\n";
		}

		inline auto to_sse(const Op& o, int level) -> std::string
		{
			return o.call([level](const auto& o){return to_sse(o, level);});
		}
	}
}
