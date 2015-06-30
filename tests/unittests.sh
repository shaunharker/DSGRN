#!/bin/bash
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $DIR
./TestDomainGraph.x
./TestMorseGraph.x
./TestOrderParameter.x
./TestMorseDecomposition.x
./TestNetwork.x
./TestParameterGraph.x
