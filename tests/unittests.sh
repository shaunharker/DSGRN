#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
./TestDomainGraph.x
./TestMorseGraph.x
./TestOrderParameter.x
./TestMorseDecomposition.x
./TestNetwork.x
./TestParameterGraph.x
dsgrn network ../networks/3D_Cycle_H.txt
dsgrn network draw
dsgrn network ../networks/3D_Cycle_H.txt domaingraph graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt wallgraph graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt morsegraph graphviz '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt domaingraph json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt wallgraph json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt morsegraph json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn network ../networks/3D_Cycle_H.txt domaingraph graphviz 234
dsgrn network ../networks/3D_Cycle_H.txt wallgraph graphviz 234
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition graphviz 234
dsgrn network ../networks/3D_Cycle_H.txt morsegraph graphviz 234
dsgrn network ../networks/3D_Cycle_H.txt domaingraph json 234
dsgrn network ../networks/3D_Cycle_H.txt wallgraph json 234
dsgrn network ../networks/3D_Cycle_H.txt morsedecomposition json 234
dsgrn network ../networks/3D_Cycle_H.txt morsegraph json 234
dsgrn parameter
dsgrn parameter list
dsgrn parameter json '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn parameter json 234
dsgrn parameter inequalities '[["X1",[3,3,"FF9E00"],[0,1,2]],["X2",[1,2,"4"],[0,1]],["X3",[2,1,"8"],[0]]]'
dsgrn parameter inequalities 234
