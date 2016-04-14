# PatternMatch

This subproject contains code which tests for patterns in DSGRN output.

## Network Specification File

For details on the format of the network specification file refer to the main DSGRN documentation.

## Pattern Specification File

As an input to the pattern matching code we require a particular JSON object which is of the following format:
```javascript
{"dimension": 7, "poset": [[7, 10, 22], [15], [5, 9, 12, 24], [17, 13, 25], [5, 9, 12, 24], [], [14, 22], [1, 19], [2], [21], [1, 19], [2, 20], [16, 3], [], [7, 10], [11, 23], [17, 13, 25], [], [14, 22], [15], [5, 9, 12, 24], [16, 3], [1, 19], [8, 20], [21], []], "events": [0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6], "label": 10287}
```

The way this works is as follows.

An event is some variable experiencing either a min or a max. We number events 0,1,2,...,M-1. Events have a poset structure. The JSON string indicates this poset structure on events as we now describe.

The "dimension" field gives the number of variables.

The "poset" field gives the list of adjacency lists. The first adjacency list gives the immediate children of event 0, the next adjacency list the immediate children of event 1, etc. For instance event 0 has the children event 7, event 10, and event 22.

The "events" field gives the list of variables associated with each event. Thus there are M numbers, e.g. data["events"][5] is the variable associated with event 5.

The "label" field indicates the final state after all events have occurred. It is a binary encoded number where the first D bits have 1's for variables which are decreasing and the next D bits have 1's for variables which are increasing, as in the following:
```python
# Final state label:
#  2*D bits, the ith bit is 1 if the final state is decreasing in variable i
#        the (i+D)th bit is 1 if the final state is increasing in variable i
label = 0b10100000101111   # variables 0,1,2,3,5 decreasing, variables 4,6 increasing
```

Note 0b10100000101111 = 10287


## Example pipeline

We describe the following computation (see `./doc/Example/Example.sh` which carries it out):
* produce networks in a suitable input format
* product experimental patterns in a suitable input format
* compute the ratio of parameters with stable FC nodes matching the experimental patterns for each network and output it in a suitable way

The input format for the networks is called a "network specification file" and we are all familiar with it.
The input format for the experimental patterns is called a "pattern specification file".

The computation currently requires the following steps. These steps are carried out in the script
```bash
./doc/Example/Example.sh
```

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

