#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
./TestDomain.x > /dev/null
./TestDomainGraph.x > /dev/null
./TestMorseGraph.x > /dev/null
./TestOrderParameter.x > /dev/null
./TestMorseDecomposition.x > /dev/null
./TestNetwork.x > /dev/null
./TestParameterGraph.x > /dev/null
dsgrn > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt > /dev/null
dsgrn network draw > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt domaingraph graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt wallgraph graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsegraph graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt domaingraph json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt wallgraph json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsegraph json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt domaingraph graphviz 234 > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt wallgraph graphviz 234 > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition graphviz 234 > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsegraph graphviz 234 > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt domaingraph json 234 > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt wallgraph json 234 > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition json 234 > /dev/null
dsgrn network ../networks/3D_Cycle_H.txt morsegraph json 234 > /dev/null
dsgrn parameter > /dev/null
dsgrn parameter list > /dev/null
dsgrn parameter json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn parameter json 234 > /dev/null
dsgrn parameter inequalities '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]' > /dev/null
dsgrn parameter inequalities 234 > /dev/null
dsgrn analyze morseset 0 0 > /dev/null

# Tests meant to fail
dsgrn fail > /dev/null || echo success > /dev/null
dsgrn parameter inequalities > /dev/null || echo success > /dev/null
#dsgrn parameter index 0 > /dev/null || echo success > /dev/null
#dsgrn domaingraph > /dev/null || echo success > /dev/null
#dsgrn wallgraph > /dev/null || echo success > /dev/null
#dsgrn morsedecomposition > /dev/null || echo success > /dev/null
#dsgrn morsegraph > /dev/null || echo success > /dev/null
