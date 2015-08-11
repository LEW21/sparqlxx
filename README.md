# sparqlxx
C++ semantic toolkit + sparqlite RDF database

## The toolkit
All the *.h files in the root directory constitute the public API of sparqlxx. They contain structures made for storing parsed SPARQL statements and their results, functions to parse and autocomplete SPARQL (require linking against libsparqlxx-parser), to serialize SPARQL Algebra to [SSE](https://jena.apache.org/documentation/notes/sse.html), and a Database class for loading and accessing concrete database plugins (like sparqlite)

The biggest single part of the toolkit is the hand-written recursive-descent SPARQL parser. It's located in parser/ and compiled into libsparqlxx-parser.so. All the .h files in parser/ are considered private, the public API is in /parse.h and /autocomplete.h.

## sparqlite
Simple implementation of SPARQL RDF database with builtin RDFS/OWL reasoner.

## The console
./bin/sparql is a console for interacting with any SPARQL database supported in a sparqlxx plugin (= currently only sparqlite). It supports history and autocompletion of SPARQL queries.

## Example

	% make
	% export LD_LIBRARY_PATH=./lib
	% ./bin/sparql
	sparqlite> PREFIX owl: <http://www.w3.org/2002/07/owl#>
		INSERT DATA {
			<isIn> a owl:TransitiveProperty.
			<Warsaw> <isIn> <Poland>.
			<Poland> <isIn> <Europe>.
		}
	OK

With the given data, it will infer that:

	</Warsaw> </isIn> </Europe>.

You can see it with:

	sparqlite> SELECT * WHERE { <Warsaw> <isIn> ?what }
	?what 
	</Poland> 
	</Europe> 

Or you can select everything, including the metadata about the reasoning process:

	sparqlite> SELECT * { ?s ?p ?o }
	?s ?p ?o 
	"</isIn> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2002/07/owl#TransitiveProperty> <urn:x-arq:DefaultGraphNode>"^^<http://xtreeme.org/#statement> <http://xtreeme.org/#source> _:x25 
	_:x25 <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://xtreeme.org/#Insertion> 
	</isIn> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2002/07/owl#TransitiveProperty> 
	"</Warsaw> </isIn> </Poland> <urn:x-arq:DefaultGraphNode>"^^<http://xtreeme.org/#statement> <http://xtreeme.org/#source> _:x31 
	_:x31 <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://xtreeme.org/#Insertion> 
	_:x40 <http://xtreeme.org/#inferredFrom> "</Poland> </isIn> </Europe> <urn:x-arq:DefaultGraphNode>"^^<http://xtreeme.org/#statement> 
	_:x40 <http://xtreeme.org/#inferredFrom> "</Warsaw> </isIn> </Poland> <urn:x-arq:DefaultGraphNode>"^^<http://xtreeme.org/#statement> 
	_:x40 <http://xtreeme.org/#inferredFrom> "</isIn> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://www.w3.org/2002/07/owl#TransitiveProperty> <urn:x-arq:DefaultGraphNode>"^^<http://xtreeme.org/#statement> 
	</Warsaw> </isIn> </Europe> 
	_:x40 <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://xtreeme.org/#Inference> 
	"</Warsaw> </isIn> </Europe> <urn:x-arq:DefaultGraphNode>"^^<http://xtreeme.org/#statement> <http://xtreeme.org/#source> _:x40 
	</Warsaw> </isIn> </Poland> 
	</Poland> </isIn> </Europe> 
	_:x36 <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://xtreeme.org/#Insertion> 
	"</Poland> </isIn> </Europe> <urn:x-arq:DefaultGraphNode>"^^<http://xtreeme.org/#statement> <http://xtreeme.org/#source> _:x36 

## License - GNU AGPLv3
Copyright (C) 2010 - 2015 Janusz Lewandowski

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

## Previous work
The project is a rewrite of [Xtreeme](https://github.com/LEW21/Xtreeme) semantic database. sparqlxx API and parser are completely new, but sparqlite reuses some (strongly refactored) Xtreeme's code.
