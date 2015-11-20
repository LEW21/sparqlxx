#include <iostream>

#include "../algebratransformers/to_quads.h"
#include "../to_sse.h"


using namespace sparqlxx;

void test_to_quads()
{
	auto op = Algebra::Op{Algebra::Null{}};

	{
		op = Algebra::Basic{{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}}};
		assert(to_sse(op) == "(bgp [<a> <b> <c>])\n");
		op = AlgebraTransformers::to_quads(std::move(op), Iri{"G"});
		assert(to_sse(op) == "(quadpattern [<G> <a> <b> <c>])\n");
	}

	{
		op = Algebra::Graph{Iri{"X"}, Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})};
		assert(to_sse(op) == "(graph <X>\n\t(bgp [<a> <b> <c>])\n)\n");
		op = AlgebraTransformers::to_quads(std::move(op), Iri{"G"});
		assert(to_sse(op) == "(quadpattern [<X> <a> <b> <c>])\n");
	}

	{
		op = Algebra::Join{Algebra::make<Algebra::Graph>(Iri{"X"}, Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})), Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})};
		assert(to_sse(op) == "(join\n\t(graph <X>\n\t\t(bgp [<a> <b> <c>])\n\t)\n\t(bgp [<a> <b> <c>])\n)\n");
		op = AlgebraTransformers::to_quads(std::move(op), Iri{"G"});
		assert(to_sse(op) == "(join\n\t(quadpattern [<X> <a> <b> <c>])\n\t(quadpattern [<G> <a> <b> <c>])\n)\n");
	}
}

int main()
{
	test_to_quads();
	return 0;
}
