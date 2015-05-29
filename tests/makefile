CFLAGS := 
CXXFLAGS := -std=c++11 -O3 -DNDEBUG -I../include
LDLIBS := -lsqlite3

%.x : %.o
	$(CXX) $(LDFLAGS) $< $(LDLIBS) -o $@

# List of targets
all: TestNetwork.x TestOrderParameter.x TestParameterGraph.x TestDomainGraph.x TestMorseDecomposition.x TestMorseGraph.x TestJSON.x

clean:
	rm -f *.x
	rm -f *.gv





