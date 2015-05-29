CXXFLAGS := -std=c++11 -O3 -DNDEBUG -I./include
LDLIBS := -lsqlite3

VPATH := source
# List of targets
all: DSGRN

test:
	$(MAKE) -C tests

install:
	cp ./DSGRN /usr/local/bin/dsgrn

clean:
	rm -f ./tests/*.x
	rm -f ./DSGRN

