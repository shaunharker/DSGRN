CFLAGS := 
CXXFLAGS := -std=c++11 -O3 -DNDEBUG -I../include
LDFLAGS := -L../lib
LDLIBS := -lsqlite3 -ldsgrn

%.x : %.o
	$(CXX) $(LDFLAGS) $< $(LDLIBS) -o $@

# List of targets
all: TestNetwork.x TestOrderParameter.x TestParameterGraph.x TestDomainGraph.x TestMorseDecomposition.x TestMorseGraph.x TestJSON.x

clean:
	rm -f *.x
	rm -f *.gv





