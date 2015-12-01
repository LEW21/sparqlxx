This is not a public API.

```c++
namespace sparqlite
{
	struct Database: public sparqlxx::DatabaseImpl
	{
		Resources res;
		Statements stmt;

		Database();
		auto query(const sparqlxx::SPARQL&) -> sparqlxx::Result override;

		template <typename T>
		auto _query(const T&) -> sparqlxx::result_of<T>;

		auto match(const sparqlxx::QuadsVP&) const -> sparqlxx::Solutions;
	};
}

extern "C" auto sparqlite_init_db(const char* iri) -> sparqlxx::DatabaseImpl*;
```

The main class of the database engine is called sparqlite::Database.
sparqlxx::DatabaseImpl interface requires it to implement one method - query().

sparqlite stores all the data in two structures - Resources and Statements.

### Resources
Resources object contains a mapping from all used RDF resources to internal sparqlite's IDs.

```c++
class Resources
{
	std::vector<Resource> byID;
	std::unordered_map<Resource, ID> byRes;

public:
	inline auto operator[](ID id) const -> Resource {return byID[id];}
	inline auto operator[](const Resource& r) const -> ID {return byRes.at(r);}
	inline auto operator[](const Resource&) -> ID;
	
	inline auto blank() -> ID;
	// ...
};
```

When non-const operator[] is used on an RDF resource that's not in the container, it gets assigned a new ID and becomes added to the container.

### Statements
Statements object contains all inserted or reasoned statements, and multiple indexes for finding them quickly.

```c++
using P1 = ID;
using P2 = std::tuple<ID, ID>;
using P3 = std::tuple<ID, ID, ID>;
using P4 = std::tuple<ID, ID, ID, ID>;

struct Statements
{
	std::unordered_set<ID> all;

	std::unordered_multimap<P1, ID> byS;
	std::unordered_multimap<P1, ID> byP;
	std::unordered_multimap<P1, ID> byO;
	std::unordered_multimap<P1, ID> byG;

	std::unordered_multimap<P2, ID> bySP;
	std::unordered_multimap<P2, ID> bySO;
	std::unordered_multimap<P2, ID> bySG;
	std::unordered_multimap<P2, ID> byPO;
	std::unordered_multimap<P2, ID> byPG;
	std::unordered_multimap<P2, ID> byOG;

	std::unordered_multimap<P3, ID> bySPO;
	std::unordered_multimap<P3, ID> bySPG;
	std::unordered_multimap<P3, ID> bySOG;
	std::unordered_multimap<P3, ID> byPOG;

	std::unordered_multimap<P4, ID> bySPOG;

	void insert(ID, const Quad&);
	void remove(ID, const Quad&);

	auto match(const Quad&) const -> std::unordered_set<ID>;
};
```

### Making INSERT DATA queries
1. All the RDF resources are mapped to resource IDs.
2. All the statements are serialized as RDF literals of type &lt;http://xtreeme.org/#statement&gt;, and mapped to resource IDs.
3. All the statements are inserted into the Statements container.

### Making DELETE DATA queries
1. All the RDF resources are mapped to resource IDs.
2. All the statements are serialized as RDF literals of type &lt;http://xtreeme.org/#statement&gt;, and mapped to resource IDs.
3. All the statements are removed from the Statements container.

### Making SELECT queries
1. Query algebra is transformed using sparqlxx::AlgebraTransformers::to_quads, which transforms all the Basic Graph Patterns made of triples to quads.
2. Algebra is optimized with sparqlxx::AlgebraTransformers::merge_joins and sparqlxx::AlgebraTransformers::merge_basics.
3. eval(dataset, algebra operation) is called.

#### eval
eval(D, op) is implemented as described in the SPARQL 1.1 Query standard. With both variant::call and C++ function overloading, we split its implementation to multiple functions - one for each Algebra operation.
Most of those functions simply eval all their suboperations, and then perform a single task on their results, like:
```c++
inline auto eval(const Dataset& D, const sparqlxx::Algebra::Reduced& op) -> sparqlxx::Solutions
{
	return reduced(eval(D, op.data));
}
```

eval(D, Algebra::Basic) and eval(D, Algebra::Graph) are left unimplemented, as they're impossible to occur after sparqlxx::AlgebraTransformers::to_quads.

eval(D, Algebra::Quad) currently finds all the solutions for each given pattern, and then join()s them. This is not the optimal way to implement it, as joining may be really costly here. It may be optimized by matching patterns with a small number of solutions first, and then matching others using already matched variables in the process.

#### Algebra tasks
##### join(std::vector<Solutions>&&) -> Solutions
Performs an inner join on multiple solutions.

##### reduced(Solutions&&) -> Solutions
Removes all consecutive duplicate elements from the solutions list.

##### project(Solutions&&, std::vector<Var>&& vars) -> Solutions
Removes columns from the result set that aren't listed in the vars vector.

##### slice(Solutions&&, int start, boost::optional<int> length) -> Solutions
Returns a <start, start+length) subsequence of the solutions list (or <start, ∞) if length is not provided).

Other operations aren't implemented yet.
