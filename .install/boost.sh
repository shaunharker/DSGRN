#!/bin/bash
wget http://downloads.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.tar.gz > /dev/null
tar xfz boost_1_58_0.tar.gz > /dev/null
cd boost_1_58_0
./bootstrap.sh --with-libraries=$1 > /dev/null
./b2 --prefix=$2 install > /dev/null
