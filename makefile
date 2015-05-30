MAJORVERSION := 0
MINORVERSION := 1
# Detect platform and make adjustments
ifeq ($(OS),Windows_NT)
    CCFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -D AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -D IA32
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -D LINUX
	SHAREDFLAG := -shared
	SHAREDEXT := so
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -D OSX
	SHAREDFLAG := -dynamiclib -undefined suppress -flat_namespace
	SHAREDEXT := dylib
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CCFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CCFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CCFLAGS += -D ARM
    endif
endif

CXXFLAGS := -std=c++11 -O3 -DNDEBUG -I./include
LDLIBS := -lsqlite3

VPATH := source
# List of targets
all: DSGRN library
	mv DSGRN ./bin/dsgrn

library:
	$(CXX) $(CXXFLAGS) -c -x c++ ./include/DSGRN.hpp -o dsgrn.o
	ar rcs libdsgrn.a dsgrn.o
	$(CXX) $(CXXFLAGS) $(SHAREDFLAG) -o libdsgrn.$(SHAREDEXT) -x c++ ./include/DSGRN.hpp
	mv libdsgrn* ./lib
	rm -f dsgrn.o

test:
	$(MAKE) -C tests

install:
	cp ./bin/dsgrn /usr/local/bin/dsgrn
	cp ./lib/libdsgrn.a /usr/local/lib/libdsgrn.a
	cp ./lib/libdsgrn.$(SHAREDEXT) /usr/local/libdsgrn.$(SHAREDEXT)

clean:
	rm -f ./tests/*.x
	rm -f ./bin/*
	rm -f ./lib/*
