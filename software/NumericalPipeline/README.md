# NumericalPipeline

## Overview

This software does the following, beginning with a file containing information about a network and a pattern to search for:

1. Extracts network_spec and pattern_spec from the input
2. Constructs the DSGRN database for the network (unless a file database.db is already present in the input folder)
3. Queries for Stable FC and obtains the associated parameter nodes
4. Uses Gibbs sampling to get parameters in the parameter nodes. Currently 10 samples per parameter node.
5. Solves ODES for each of the sampled parameters and creates “simulation summaries”
6. Tests each simulation summary against the pattern to see if it matches
7. Outputs a list of matches, each containing information about parameter node index, sampled parameter, and the simulation summary

## Usage

```bash
./NumericalPipeline.sh /path/to/data/folder
```
where `/path/to/data/folder` is a path to a data folder containing a file named "NetworkAndPattern.txt" with contents as described in the "Input file" section below.

## Input file

As an input, we begin with a folder (named anything we like) which we call the _data folder_ containing a file named "NetworkAndPattern.txt" with contents of the following form.
```javascript
{"MultistableParameterCount": 1, "StableFCMatchesParameterCount": 7020, "Network": "PF3D7_0611200 : (PF3D7_1337100) : E\nPF3D7_1139300 : (~PF3D7_0611200) : E\nPF3D7_1146600 : (~PF3D7_1139300)(~PF3D7_1408200) : E\nPF3D7_1222600 : (PF3D7_1146600)(~PF3D7_1317200) : E\nPF3D7_1317200 : (PF3D7_1408200)(~PF3D7_1356900) : E\nPF3D7_1337100 : (PF3D7_1139300 + PF3D7_1317200) : E\nPF3D7_1356900 : (~PF3D7_1222600) : E\nPF3D7_1408200 : (~PF3D7_0611200)(~PF3D7_1356900) : E\n", "StableFCParameterCount": 8400, "ParameterCount": 12544, "Pattern": {"dimension": 8, "poset": [[9, 12], [14], [7], [9, 12], [9, 12], [7], [14], [], [7], [11], [7], [1, 6, 13], [11], [14], [8, 2, 10, 5], [9, 12]], "events": [0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7], "label": 45645}}
```
Note the only required fields are "Network" and "Pattern"; all other fields will be ignored.

For backwards compatibility reasons the code also accepts the input format
```python
{u'MultistableParameterCount': 1, u'StableFCMatchesParameterCount': 7020, u'Network': u'PF3D7_0611200 : (PF3D7_1337100) : E\nPF3D7_1139300 : (~PF3D7_0611200) : E\nPF3D7_1146600 : (~PF3D7_1139300)(~PF3D7_1408200) : E\nPF3D7_1222600 : (PF3D7_1146600)(~PF3D7_1317200) : E\nPF3D7_1317200 : (PF3D7_1408200)(~PF3D7_1356900) : E\nPF3D7_1337100 : (PF3D7_1139300 + PF3D7_1317200) : E\nPF3D7_1356900 : (~PF3D7_1222600) : E\nPF3D7_1408200 : (~PF3D7_0611200)(~PF3D7_1356900) : E\n', u'StableFCParameterCount': 8400, u'ParameterCount': 12544, u'PartialOrder': u'{"dimension": 8, "poset": [[9, 12], [14], [7], [9, 12], [9, 12], [7], [14], [], [7], [11], [7], [1, 6, 13], [11], [14], [8, 2, 10, 5], [9, 12]], "events": [0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7], "label": 45645}'}
```

## Output files

Note: All output files are stored in the data folder alongside the initial `NetworkAndPattern.txt` file.

### NetworkAndPattern JSON file

The input file `NetworkAndPattern.txt` is parsed and put into the expected JSON format (this may be a do-nothing operation if it is already in the correct format) which is stored into `NetworkAndPattern.json`.

### Network Specification File

From the input the network specification file is extracted and stored as `network_spec.txt`

### Pattern Specification File

From the input the pattern specification file is extracted and stored as `pattern_spec.txt`

### DSGRN Database

The output contains a file `database.db` which is the DSGRN database (of sqlite3 format) computed for the network given in the input.

### StableFC matches

An SQL query is performed on the `database.db` DSGRN database to extract a list of parameter indices corresponding to parameter nodes for which the computed Morse graph has a Stable FC node. These matches are stored in a file `StableFC.txt` which contain lines with pairs (parameter index, Morse set index). The first column of this file is extracted and the unique parameter indices are saved in a file `parameter_indices.txt`

### Sampled Parameters

A Gibbs sampling program (ParameterSampler) samples parameters from the parameter nodes specified in the file `parameter_indices.txt`. The results are stored in the file `parameters.json`.

### Simulation Database

The SimulationDatabase program takes the `parameters.json` file as input and produces qualititative summaries for each parameter appearing in this file.

These summaries are stored in a file called `simulation_database.json` which has the following format:
```javascript
{"ParameterIndex": 10223, "SimulationSummary": [3570, 3315, 35955, 36465, 40545, 39525, 47685, 45645, 45900, 45390, 12750, 13770, 30090, 26010, 17850, 19890, 3570, 3315, 35955, 36465, 40545, 39525, 47685, 45645, 45900, 45390, 12750, 13770, 30090, 26010, 17850, 19890, 3570, 3315], "Parameter": {"L[PF3D7_1139300, PF3D7_1337100]": 0.06919109512671918, "L[PF3D7_1222600, PF3D7_1356900]": 0.14585197311246362, "U[PF3D7_0611200, PF3D7_1408200]": 2.58201924992486, "U[PF3D7_1146600, PF3D7_1222600]": 1.7946168110847573, "L[PF3D7_1356900, PF3D7_1408200]": 0.2431731686158545, "L[PF3D7_0611200, PF3D7_1139300]": 0.5216448120990161, "U[PF3D7_1337100, PF3D7_0611200]": 1.5878226354777474, "T[PF3D7_1356900, PF3D7_1408200]": 0.325712401798444, "L[PF3D7_1408200, PF3D7_1317200]": 0.027004449323960768, "T[PF3D7_1139300, PF3D7_1337100]": 0.6591190365336171, "L[PF3D7_1317200, PF3D7_1337100]": 0.04969117541577463, "L[PF3D7_1317200, PF3D7_1222600]": 1.5029981586126797, "T[PF3D7_1356900, PF3D7_1317200]": 0.4181753670064232, "L[PF3D7_1337100, PF3D7_0611200]": 0.4267178316314163, "U[PF3D7_1317200, PF3D7_1222600]": 5.1141076097668465, "T[PF3D7_1146600, PF3D7_1222600]": 0.7666474618025401, "U[PF3D7_1408200, PF3D7_1146600]": 1.5195865003039093, "U[PF3D7_1139300, PF3D7_1146600]": 3.8557688518137843, "T[PF3D7_1337100, PF3D7_0611200]": 0.3560266088893756, "U[PF3D7_1356900, PF3D7_1408200]": 0.7707444046746715, "T[PF3D7_1317200, PF3D7_1337100]": 0.6485977566104418, "U[PF3D7_1317200, PF3D7_1337100]": 2.5934525284553938, "T[PF3D7_1408200, PF3D7_1317200]": 0.8962153014739203, "T[PF3D7_1317200, PF3D7_1222600]": 0.6441329583594766, "L[PF3D7_1146600, PF3D7_1222600]": 0.5143374819942661, "L[PF3D7_1356900, PF3D7_1317200]": 0.32728529993321936, "L[PF3D7_1408200, PF3D7_1146600]": 0.19900016350681315, "T[PF3D7_1222600, PF3D7_1356900]": 0.9060611405593703, "U[PF3D7_1139300, PF3D7_1337100]": 0.34802858747198284, "L[PF3D7_0611200, PF3D7_1408200]": 0.09730492277910845, "T[PF3D7_1139300, PF3D7_1146600]": 1.3546261019383545, "T[PF3D7_1408200, PF3D7_1146600]": 1.8224520856158384, "T[PF3D7_0611200, PF3D7_1139300]": 1.02636237518543, "U[PF3D7_0611200, PF3D7_1139300]": 1.8490910322653502, "U[PF3D7_1356900, PF3D7_1317200]": 2.0800971779840878, "U[PF3D7_1408200, PF3D7_1317200]": 1.4258093075609641, "L[PF3D7_1139300, PF3D7_1146600]": 1.8398024038715008, "U[PF3D7_1222600, PF3D7_1356900]": 1.5954405889743593, "T[PF3D7_0611200, PF3D7_1408200]": 1.234312630091311}}
{"ParameterIndex": 10231, "SimulationSummary": [19635, 3315, 35955, 36465, 44625, 43605, 47685, 45645, 45900, 45390, 12750, 13770, 9690, 26010, 17850, 17595, 19635, 3315, 35955, 36465, 44625, 43605, 47685, 45645, 45900, 12750, 13770, 9690, 26010, 17850, 17595, 19635, 3315, 35955, 36465], "Parameter": {"L[PF3D7_1139300, PF3D7_1337100]": 0.4547399621873438, "L[PF3D7_1222600, PF3D7_1356900]": 0.13339502856289004, "U[PF3D7_0611200, PF3D7_1408200]": 2.5620860915849586, "U[PF3D7_1146600, PF3D7_1222600]": 1.9899268520456888, "L[PF3D7_1356900, PF3D7_1408200]": 0.26684086267724505, "L[PF3D7_0611200, PF3D7_1139300]": 0.9484975224461262, "U[PF3D7_1337100, PF3D7_0611200]": 1.7354055822810666, "T[PF3D7_1356900, PF3D7_1408200]": 0.21019724067673365, "L[PF3D7_1408200, PF3D7_1317200]": 0.1383505975790594, "T[PF3D7_1139300, PF3D7_1337100]": 1.0964776773880933, "L[PF3D7_1317200, PF3D7_1337100]": 0.5017111550070751, "L[PF3D7_1317200, PF3D7_1222600]": 0.8049033502973313, "T[PF3D7_1356900, PF3D7_1317200]": 1.3800183402174102, "L[PF3D7_1337100, PF3D7_0611200]": 0.16974228803865934, "U[PF3D7_1317200, PF3D7_1222600]": 8.344663458485915, "T[PF3D7_1146600, PF3D7_1222600]": 0.11300948138018115, "U[PF3D7_1408200, PF3D7_1146600]": 2.718262878581536, "U[PF3D7_1139300, PF3D7_1146600]": 1.5428006049115952, "T[PF3D7_1337100, PF3D7_0611200]": 1.5679233523568665, "U[PF3D7_1356900, PF3D7_1408200]": 2.4018707972795785, "T[PF3D7_1317200, PF3D7_1337100]": 1.196250402529026, "U[PF3D7_1317200, PF3D7_1337100]": 1.2592567959476377, "T[PF3D7_1408200, PF3D7_1317200]": 0.712334247275838, "T[PF3D7_1317200, PF3D7_1222600]": 0.49100089974836103, "L[PF3D7_1146600, PF3D7_1222600]": 0.6644421783461432, "L[PF3D7_1356900, PF3D7_1317200]": 0.2125021967738038, "L[PF3D7_1408200, PF3D7_1146600]": 0.3982221286521779, "T[PF3D7_1222600, PF3D7_1356900]": 1.215567847410069, "U[PF3D7_1139300, PF3D7_1337100]": 2.054437178234765, "L[PF3D7_0611200, PF3D7_1408200]": 0.1368339206151067, "T[PF3D7_1139300, PF3D7_1146600]": 3.1757570441102136, "T[PF3D7_1408200, PF3D7_1146600]": 1.6539799114676466, "T[PF3D7_0611200, PF3D7_1139300]": 0.2828369876824619, "U[PF3D7_0611200, PF3D7_1139300]": 6.763989056201729, "U[PF3D7_1356900, PF3D7_1317200]": 2.8719646297162393, "U[PF3D7_1408200, PF3D7_1317200]": 2.5218461070323706, "L[PF3D7_1139300, PF3D7_1146600]": 0.2504360300093885, "U[PF3D7_1222600, PF3D7_1356900]": 1.7973096410023621, "T[PF3D7_0611200, PF3D7_1408200]": 0.34068138835338674}}
...
```

Each line contains a JSON string representing the result of a simulation. It contains the parameter index (which identifies a parameter node), a "Parameter", and a vector of labels called a "SimulationSummary".

### Matched Simulation Database

The "NumericalSimulationPatternMatch" program takes teh `simulation_database.json` file as input and produces the output file `matched_simulation_database.json` which contains a subset of the lines of `simulation_database.json` corresponding to the records which matched the pattern specified in the original file.

## Analysis of Results

In order to see how many matches were found, we can type

```bash
wc matched_simulation_database.json
```
which will give the number of matches found.

Not every match need be associated with a unique parameter node, however. In order to learn how many matching parameter nodes were found, instead we can type

```bash
sed 's/^.*Index\": \([0-9]*\).*/\1/g' matched_simulation_database.json | sort -n | uniq | wc
```

