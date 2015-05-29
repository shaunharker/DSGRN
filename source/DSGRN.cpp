/// DSGRN.cpp
/// Shaun Harker
/// 2015-05-24

#include <iostream>
#include <vector>
#include <cstdlib>
#include <memory>
#include <string>

#include "Parameter/ParameterGraph.h"
#include "Phase/DomainGraph.h"
#include "Phase/WallGraph.h"
#include "Dynamics/MorseDecomposition.h"
#include "Dynamics/MorseGraph.h"

std::shared_ptr<Network> network ( new Network );

void helpmessage ( void ) {
  std::cout << " DSGRN (Dynamic Signatures for Gene Regulatory Networks)\n"
               "     [experimental interface, subject to change]\n"
               " Usage: \n"
               "   dsgrn network NETWORKFILE \n"
               "      This enables omission of the network file in subsequent commands.\n"
               "      The network is stored to a file called \"dsgrn.session\" in "
               "      the current working directory. This file can be deleted without\n"
               "      consequences.\n\n"
               "   dsgrn [network NETWORKFILE] parameter list\n\n"
               "      Produces the full (potentially far too long) list of \n" 
               "      parameters for the given network.\n\n"
               "   dsgrn [network NETWORKFILE] parameter (inequalities|json|index) PARAMETER\n"
               "      Produces the parameter inequalities, json string, or index \n"
               "      corresponding to the provided parameter\n\n"
               "   dsgrn [network NETWORKFILE] domaingraph (graphviz|json) PARAMETER\n"
               "      Computes the domain graph corresponding to the\n" 
               "      provided parameter and outputs in specified form.\n\n"
               "   dsgrn [network NETWORKFILE] wallgraph (graphviz|json) PARAMETER\n"
               "      Computes the wall graph corresponding to the\n" 
               "      provided parameter and outputs in specified form.\n\n"
               "   dsgrn [network NETWORKFILE] morsedecomposition (graphviz|json) PARAMETER\n"
               "      Computes the Morse decomposition corresponding to the\n"
               "      provided parameter and outputs in specified form.      \n\n"
               "   dsgrn [network NETWORKFILE] morsegraph (graphviz|json) PARAMETER\n"
               "      Computes the Morse graph corresponding to the\n"
               "      provided parameter and outputs in specified form.      \n\n"
               " Notes:  The notation [] means optional. The notation (a|b|c) means to\n"
               "         write either a, b, or c.\n"
               "         NETWORKFILE indicates the filename of the network file. \n"
               "         PARAMETER may either be an integer index or else the json \n"
               "         specification. In the case of JSON, enclose the string in \n"
               "         single quotes to avoid the shell misinterpreting the string.\n"
               " Hint: To save results to a file, use the redirection command >, i.e. \n"
               "       # dsgrn morsegraph 932 > morsegraph.gv \n";
}



std::shared_ptr<Parameter> 
parse_parameter ( std::string const& s ) {
  std::shared_ptr<Parameter> p;
  if ( s[0] >= '0' && s[0] <= '9' ) {
    ParameterGraph pg;
    pg . assign ( network, "./data/logic/" );
    p = pg . parameter ( std::stoll(s) );   
  } else {
    p . reset ( new Parameter );
    p -> assign ( network );
    p -> parse ( s );
  }
  return p;
}

void parameter ( int argc, char * argv [] ) {
  if ( argc == 2 ) {
    ParameterGraph pg;
    pg . assign ( network, "./data/logic/" );
    uint64_t N = pg . size ();
    std::cout << "There are " << N << " parameters.\n";
    return;
  }
  std::string command = argv[2];
  if ( command == "list" ) {
    ParameterGraph pg;
    pg . assign ( network, "./data/logic/" );
    uint64_t N = pg . size ();
    for ( uint64_t i = 0; i < N; ++ i ) {
      std::shared_ptr<Parameter> p = pg . parameter ( i );
      std::cout << * p << "\n";
    }
    return;
  }
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  std::shared_ptr<Parameter> p = parse_parameter ( s );
  if ( command == "inequalities" ) {
    std::cout << p -> inequalities ();
  }
  if ( command == "json" ) {
    std::cout << p -> stringify ();
  }
  if ( command == "index" ) {
    ParameterGraph pg;
    pg . assign ( network, "./data/logic/" );
    std::cout << pg . index ( p );
  }
}

void domaingraph ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  std::shared_ptr<Parameter> p = parse_parameter ( s );
  std::shared_ptr<DomainGraph> dg ( new DomainGraph );
  dg -> assign ( p );
  if ( command == "json" ) {
    std::cout << dg -> digraph() -> stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << *dg;
  } 
}

void wallgraph ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  std::shared_ptr<Parameter> p = parse_parameter ( s );
  std::shared_ptr<WallGraph> wg ( new WallGraph );
  wg -> assign ( p );
  if ( command == "json" ) {
    std::cout << wg -> digraph() -> stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << *wg;
  } 
}


void morsedecomposition ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  std::shared_ptr<Parameter> p = parse_parameter ( s );
  std::shared_ptr<DomainGraph> dg ( new DomainGraph );
  dg -> assign ( p );
  std::shared_ptr<MorseDecomposition> md ( new MorseDecomposition );
  md -> assign ( dg -> digraph () );
  if ( command == "json" ) {
    std::cout << md -> poset() -> stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << *md;
  } 
}

void morsegraph ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  std::shared_ptr<Parameter> p = parse_parameter ( s );
  std::shared_ptr<DomainGraph> dg ( new DomainGraph );
  dg -> assign ( p );
  std::shared_ptr<MorseDecomposition> md ( new MorseDecomposition );
  md -> assign ( dg -> digraph () );
  MorseGraph mg;
  mg . assign ( dg, md );
  if ( command == "json" ) {
    std::cout << mg . stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << mg;
  } 
}

int load_network_from_session ( void ) {
  std::ifstream infile ( "dsgrn.session" );
  if ( not infile ) {
    std::cout << "No network specified. Call without command arguments for help.\n";
    return 1;
  }
  std::string filename;
  std::getline ( infile, filename );
  infile . close ();
  network -> load ( filename );
  return 0;
}

int main ( int argc, char * argv [] ) {

  if ( argc == 1 ) {
    helpmessage ();
    return 0;
  }

  std::string command ( argv[1] );
  if ( command == "help" ) {
    helpmessage ();
    return 0;
  }

  if ( command == "network" ) {
    if ( argc == 2 ) {
      helpmessage ();
      return 0;
    }
    std::string filename = argv[2];
    if ( filename == "graphviz" || filename == "draw" ) {
      int rc = load_network_from_session ();
      if ( rc == 1 ) return rc;
      std::cout << network -> graphviz ();
      return 0;
    }
    //std::cout << "loading network " << filename << "\n";
    network -> load ( filename );
    //std::cout << "setting environment variable...\n";
    std::ofstream outfile ( "dsgrn.session" );
    outfile << filename << "\n";
    outfile . close ();
    argc -= 2; argv += 2;
    if ( argc == 1 ) return 0;
    command = argv[1];
    if ( command == "graphviz" ) {
      std::cout << network -> graphviz ();
      return 0;
    }
  } else {
    int rc = load_network_from_session ();
    if ( rc == 1 ) return rc;
  }


  if ( (command == "p" ) ||
       (command == "parameter") ) {
    parameter ( argc, argv );
    return 0;
  }
  if ((command == "DG") ||
      (command == "dg") ||
      (command == "DomainGraph") ||
      (command == "domaingraph") ) {
    domaingraph ( argc, argv );
    return 0;
  }
  if ((command == "WG") ||
      (command == "wg") ||
      (command == "WallGraph") ||
      (command == "wallgraph") ) {
    wallgraph ( argc, argv );
    return 0;
  }
  if ((command == "MD") ||
      (command == "md") ||
      (command == "MorseDecomposition") ||
      (command == "morsedecomposition") ) {
    morsedecomposition ( argc, argv );
    return 0;
  }
  if ((command == "MG") ||
      (command == "mg") ||
      (command == "MorseGraph") ||
      (command == "morsegraph") ) {
    morsegraph ( argc, argv );
    return 0;
  }

  std::cout << "Unrecognized command \"" << command << "\"\n";
  return 1;
}