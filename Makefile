CXX=clang++
CXXFLAGS=-Wall -Werror -Wextra -pedantic -Wno-char-subscripts -Wno-sign-compare -Wno-unknown-pragmas -Wno-error=unused-parameter -g -std=c++14 -Iuri/src/ -fdiagnostics-color -Wl,-E
LIBFLAGS=-shared -fPIC -fvisibility=hidden
LIBS=uri/_build/src/libnetwork-uri.a -lboost_system
LPARSER=-Llib -lsparqlxx-parser
LREADLINE=-lreadline -DUSE_READLINE
#LREADLINE=

.PHONY: test test_parser_internal test_parse test_database_sparqlite clean

all: lib/libsparqlxx-parser.so lib/libsparqlite.so bin/sparql_to_sse bin/sparql test

test: test_parser_internal test_parse test_transformers test_engine_sparqlite test_sparqlite_internal

test_parser_internal: bin/test_parser_internal
	./bin/test_parser_internal

test_parse: bin/test_parse
	LD_LIBRARY_PATH=lib ./bin/test_parse

test_transformers: bin/test_transformers
	LD_LIBRARY_PATH=lib ./bin/test_transformers

test_engine_sparqlite: bin/test_engine
	LD_LIBRARY_PATH=lib ./bin/test_engine sparqlite

test_database_sparqlite: bin/test_database
	LD_LIBRARY_PATH=lib ./bin/test_database sparqlite

test_sparqlite_internal: bin/test_sparqlite_internal
	LD_LIBRARY_PATH=lib ./bin/test_sparqlite_internal

lib/libsparqlxx-parser.so: *.h parser/*.h parser/tokenize.cpp parser/parse.cpp parser/read_*.cpp
	@test -d lib/ || mkdir -p lib/
	$(CXX) $(CXXFLAGS) $(LIBFLAGS) parser/tokenize.cpp parser/parse.cpp parser/read_*.cpp -Wl,-soname,libsparqlxx-parser.so.0 -o ./lib/libsparqlxx-parser.so.0
	rm -f ./lib/libsparqlxx-parser.so
	ln -s libsparqlxx-parser.so.0 ./lib/libsparqlxx-parser.so

bin/test_parser_internal: *.h parser/*.h parser/tokenize.cpp parser/parse.cpp parser/read_*.cpp parser/test.cpp
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) parser/*.cpp $(LIBS) -o bin/test_parser_internal

lib/libsparqlite.so: *.h algebratransformers/*.h lib/libsparqlxx-parser.so sparqlite/*.h sparqlite/*.cpp
	@test -d lib/ || mkdir -p lib/
	$(CXX) $(CXXFLAGS) $(LIBFLAGS) sparqlite/database.cpp sparqlite/query.cpp sparqlite/update.cpp sparqlite/think.cpp $(LPARSER) -lboost_serialization -Wl,-soname,libsparqlite.so.0 -o ./lib/libsparqlite.so.0
	rm -f ./lib/libsparqlite.so
	ln -s libsparqlite.so.0 ./lib/libsparqlite.so

bin/sparql_to_sse: lib/libsparqlxx-parser.so algebratransformers/*.h sparql_to_sse/main.cpp
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) sparql_to_sse/main.cpp $(LPARSER) $(LIBS) -o bin/sparql_to_sse

bin/sparql: *.h lib/libsparqlxx-parser.so lib/libsparqlite.so console/main.cpp
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) console/main.cpp -ldl $(LPARSER) $(LREADLINE) $(LIBS) -o bin/sparql

bin/test_parse: tests/test_parse.cpp lib/libsparqlxx-parser.so
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) tests/test_parse.cpp $(LPARSER) $(LIBS) -o bin/test_parse

bin/test_database: *.h tests/test_database.cpp
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) tests/test_database.cpp -ldl $(LPARSER) $(LIBS) -o bin/test_database

bin/test_transformers: *.h algebratransformers/*.h tests/test_transformers.cpp
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) tests/test_transformers.cpp -ldl $(LIBS) -o bin/test_transformers

bin/test_engine: *.h tests/test_engine.cpp
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) tests/test_engine.cpp -ldl $(LPARSER) $(LIBS) -o bin/test_engine

bin/test_sparqlite_internal: *.h sparqlite/*.h sparqlite/test.cpp
	@test -d bin/ || mkdir -p bin/
	$(CXX) $(CXXFLAGS) sparqlite/test.cpp -ldl $(LPARSER) $(LIBS) -lboost_serialization -lsparqlite -o bin/test_sparqlite_internal

DEF_INCLUDEPATH=-I/usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/5.2.0/../../../../include/c++/5.2.0 -I/usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/5.2.0/../../../../include/c++/5.2.0/x86_64-unknown-linux-gnu -I/usr/bin/../lib64/gcc/x86_64-unknown-linux-gnu/5.2.0/../../../../include/c++/5.2.0/backward -I/usr/local/include -I/usr/bin/../lib/clang/3.7.0/include -I/usr/include

rdoc:
	cldoc generate $(CXXFLAGS) -w -DSPARQLXX_DOC -- *.h --output doc

xdoc:
	cldoc generate $(CXXFLAGS) -w -DSPARQLXX_DOC -- *.h --output doc --static

tdoc:
	cldoc generate $(CXXFLAGS) -w -DSPARQLXX_DOC -- algebr*.h variantxx/variant.hpp --output xxx

clean:
	rm -rf bin lib
