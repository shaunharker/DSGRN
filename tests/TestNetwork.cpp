/// TestNetwork.cpp
/// Shaun Harker
/// 2015-05-22

#include <iostream>
#include <string>
#include <fstream>

#include "Parameter/Network.h"

int main ( int argc, char * argv [] ) {
  // Load network
  Network network;
  std::string testfile;
  if ( argc < 2 ) testfile = "networks/network1.txt";
  else testfile = argv[1];
  network . load ( testfile );

  // Save network graphviz to disk
  std::ofstream outfile ( "testnetwork.gv");
  outfile << network . graphviz ();
  outfile . close ();
  return 0;
}
