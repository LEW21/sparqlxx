#include <iostream>

#include "../algebratransformers/to_quads.h"
#include "../algebratransformers/merge_joins.h"
#include "../algebratransformers/merge_basics.h"
#include "../to_sse.h"
#include <boost/algorithm/string/replace.hpp>

using namespace sparqlxx;

template <typename T>
auto to_unformatted_sse(const T& val)
{
	auto s = to_sse(val);
	boost::algorithm::replace_all(s, "\t", "");
	boost::algorithm::replace_all(s, "\n", " ");
	s = s.substr(0, s.size()-1);
	return s;
}

void test_to_quads()
{
	auto op = Algebra::Op{Algebra::Null{}};

	{
		op = Algebra::Basic{{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}}};
		assert(to_unformatted_sse(op) == "(bgp [<a> <b> <c>])");
		op = AlgebraTransformers::to_quads(std::move(op), Iri{"G"});
		assert(to_unformatted_sse(op) == "(quadpattern [<G> <a> <b> <c>])");
	}

	{
		op = Algebra::Graph{Iri{"X"}, Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})};
		assert(to_unformatted_sse(op) == "(graph <X> (bgp [<a> <b> <c>]) )");
		op = AlgebraTransformers::to_quads(std::move(op), Iri{"G"});
		assert(to_unformatted_sse(op) == "(quadpattern [<X> <a> <b> <c>])");
	}

	{
		op = Algebra::Join{Algebra::make<Algebra::Graph>(Iri{"X"}, Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})), Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})};
		assert(to_unformatted_sse(op) == "(join (graph <X> (bgp [<a> <b> <c>]) ) (bgp [<a> <b> <c>]) )");
		op = AlgebraTransformers::to_quads(std::move(op), Iri{"G"});
		assert(to_unformatted_sse(op) == "(join (quadpattern [<X> <a> <b> <c>]) (quadpattern [<G> <a> <b> <c>]) )");
	}
}

void test_merge_joins()
{
	auto op = Algebra::Op{Algebra::Null{}};

	{
		op = Algebra::Join{
			Algebra::make<Algebra::Graph>(Iri{"X"},
				Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})
			),
			Algebra::make<Algebra::Join>(
				Algebra::make<Algebra::Join>(
					Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"d"}}}),
					Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"e"}}})
				),
				Algebra::make<Algebra::Join>(
					Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"f"}}}),
					Algebra::make<Algebra::Null>()
				)
			)
		};
		assert(to_unformatted_sse(op) == "(join (graph <X> (bgp [<a> <b> <c>]) ) (join (join (bgp [<a> <b> <d>]) (bgp [<a> <b> <e>]) ) (join (bgp [<a> <b> <f>]) (table unit) ) ) )");
		op = AlgebraTransformers::merge_joins(std::move(op));
		assert(to_unformatted_sse(op) == "(join (graph <X> (bgp [<a> <b> <c>]) ) (bgp [<a> <b> <d>]) (bgp [<a> <b> <e>]) (bgp [<a> <b> <f>]) )");
//		std::cout << to_unformatted_sse(op) << std::endl;
	}
}

void test_merge_basics()
{
	auto op = Algebra::Op{Algebra::Null{}};

	{
		op = Algebra::Join{
			Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"d"}}}),
			Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"e"}}}),
			Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"f"}}})
		};
		assert(to_unformatted_sse(op) == "(join (bgp [<a> <b> <d>]) (bgp [<a> <b> <e>]) (bgp [<a> <b> <f>]) )");
		op = AlgebraTransformers::merge_basics(std::move(op));
		assert(to_unformatted_sse(op) == "(bgp  [<a> <b> <d>] [<a> <b> <e>] [<a> <b> <f>] )");
	}
}

void test_qjb()
{
	auto op = Algebra::Op{Algebra::Null{}};

	{
		op = Algebra::Join{
			Algebra::make<Algebra::Graph>(Iri{"X"},
				Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"c"}}})
			),
			Algebra::make<Algebra::Join>(
				Algebra::make<Algebra::Join>(
					Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"d"}}}),
					Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"e"}}})
				),
				Algebra::make<Algebra::Join>(
					Algebra::make<Algebra::Basic>(TriplesVP{TripleVP{Iri{"a"}, Iri{"b"}, Iri{"f"}}}),
					Algebra::make<Algebra::Null>()
				)
			)
		};
		assert(to_unformatted_sse(op) == "(join (graph <X> (bgp [<a> <b> <c>]) ) (join (join (bgp [<a> <b> <d>]) (bgp [<a> <b> <e>]) ) (join (bgp [<a> <b> <f>]) (table unit) ) ) )");
		op = AlgebraTransformers::merge_basics(AlgebraTransformers::merge_joins(AlgebraTransformers::to_quads(std::move(op), Iri{"G"})));
		assert(to_unformatted_sse(op) == "(quadpattern  [<X> <a> <b> <c>] [<G> <a> <b> <d>] [<G> <a> <b> <e>] [<G> <a> <b> <f>] )");
	}
}

int main()
{
	test_to_quads();
	test_merge_joins();
	test_merge_basics();
	test_qjb();
	return 0;
}
