CC := mpicxx
CXX := mpicxx
CXXFLAGS := -std=c++11 -O3 -DNDEBUG -I./include -I../../include -ftemplate-depth=2048
LDFLAGS := -L../../lib
LDLIBS := -lsqlite3 -lboost_serialization -ldsgrn

# List of targets
VPATH = ./source
all: Signatures


clean:
	rm -f Signatures
