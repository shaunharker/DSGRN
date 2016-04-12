# PatternMatch

We describe the following computation:
* produce networks in a suitable input format
* product experimental patterns in a suitable input format
* compute the ratio of parameters with stable FC nodes matching the experimental patterns for each network and output it in a suitable way

The input format for the networks is called a "network specification file" and we are all familiar with it.
The input format for the experimental patterns is called a "pattern specification file" and I gave an example in an earlier e-mail.

The computation currently requires the following steps
First, acquire the patternmatch-dev branch of the code and compile it. Then:

a) Produce a network specification file `network_spec.txt` representing the network of interest

b) Produce a pattern specification file `pattern_spec.json` representing the min/max sequences of interest

c) Run "dsgrn" to determine the total number of parameter nodes
```bash
dsgrn network /path/to/network_spec.txt parameter
```

d) Run "Signatures" to produce the DSGRN database file `database.db`
```bash
cd /path/to/DSGRN/software/Signatures
mpiexec -np 9 ./bin/Signatures /path/to/network_spec.txt /path/to/database.db
```

e) Perform an SQL query to produce a file with the (parameter index, morse set index) pairs corresponding to stable FC nodes.
```bash
cd /path/to/DSGRN/software/PatternMatch
./bin/StableFCQuery.sh /path/to/database.db /path/to/StableFCList.txt
```
The first argument is an input and the second argument is the output.

f) Run the "PatternMatchDatabase" program, which takes a network specification file and a list of (parameter index, morse set index) pairs and returns the sublist for which the match took place

```bash
cd /path/to/DSGRN/software/PatternMatch
mpiexec -np 9 ./bin/PatternMatchDatabase /path/to/network_spec.txt /path/to/pattern_spec.json /path/to/StableFCList.txt /path/to/Matches.txt
```

The first three arguments are inputs and the fourth argument is the output

g) From the Matches.txt file and the number of parameters total (see (c)) the ratio can be determined. Note there is a possibility that there are two stable FCs for a single parameter index which could result in double counting. To prevent this, we can get the count of parameter nodes with a match without double counting with the following line (in bash):

```bash
cut -d " " -f 1 Matches.txt | sort | uniq | wc -w
```

