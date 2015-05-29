CC := mpicxx
CXX := mpicxx
CXXFLAGS := -std=c++11 -O3 -DNDEBUG -I./include -I../../include
LDLIBS := -lsqlite3 -lboost_serialization

# List of targets
VPATH = ./source
all: Signatures


clean:
	rm -f Signatures
