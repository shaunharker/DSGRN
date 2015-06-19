/// DSGRN.cpp
/// Shaun Harker
/// 2015-05-24

#include "common.h"
#include "DSGRN.h"

Network network;

void helpmessage ( void ) {
  std::cout << " DSGRN (Dynamic Signatures for Gene Regulatory Networks)\n"
               "     [experimental interface, subject to change]\n"
               " Usage: \n"
               "   dsgrn network NETWORKFILE \n"
               "      This enables omission of the network file in subsequent commands.\n"
               "      The network is stored to a file called \"dsgrn.session\" in \n"
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
               "   dsgrn [network NETWORKFILE] analyze morseset NUMBER PARAMETER\n"
               "      Returns a digraph associated with the indicated Morse set along\n"
               "      with other information\n"
               " Notes:  The notation [] means optional. The notation (a|b|c) means to\n"
               "         write either a, b, or c.\n"
               "         NETWORKFILE indicates the filename of the network file. \n"
               "         PARAMETER may either be an integer index or else the json \n"
               "         specification. In the case of JSON, enclose the string in \n"
               "         single quotes to avoid the shell misinterpreting the string.\n"
               " Hint: To save results to a file, use the redirection command >, i.e. \n"
               "       # dsgrn morsegraph 932 > morsegraph.gv \n";
}



Parameter
parse_parameter ( std::string const& s ) {
  Parameter p;
  if ( s[0] >= '0' && s[0] <= '9' ) {
    ParameterGraph pg ( network );
    p = pg . parameter ( std::stoll(s) );   
  } else {
    p = Parameter ( network );
    p . parse ( s );
  }
  return p;
}

void parameter ( int argc, char * argv [] ) {
  if ( argc == 2 ) {
    ParameterGraph pg ( network );
    uint64_t N = pg . size ();
    std::cout << "There are " << N << " parameters.\n";
    return;
  }
  std::string command = argv[2];
  if ( command == "list" ) {
    ParameterGraph pg ( network );
    uint64_t N = pg . size ();
    for ( uint64_t i = 0; i < N; ++ i ) {
      Parameter p = pg . parameter ( i );
      std::cout << p << "\n";
    }
    return;
  }
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  Parameter p = parse_parameter ( s );
  if ( command == "inequalities" ) {
    std::cout << p . inequalities ();
  }
  if ( command == "json" ) {
    std::cout << p . stringify ();
  }
  if ( command == "index" ) {
    ParameterGraph pg ( network );
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
  Parameter p = parse_parameter ( s );
  DomainGraph dg ( p );
  if ( command == "json" ) {
    std::cout << dg . digraph() . stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << dg;
  } 
}

void wallgraph ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  Parameter p = parse_parameter ( s );
  WallGraph wg ( p );
  if ( command == "json" ) {
    std::cout << wg . digraph() . stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << wg;
  } 
}


void morsedecomposition ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  Parameter p = parse_parameter ( s );
  DomainGraph dg ( p );
  MorseDecomposition md;
  md . assign ( dg . digraph () );
  if ( command == "json" ) {
    std::cout << md . poset() . stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << md;
  } 
}

void morsegraph ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( argc == 3 ) {
    std::cout << "No parameter supplied.\n";
    return;
  }
  std::string s = argv[3];
  Parameter p = parse_parameter ( s );
  DomainGraph dg ( p );
  MorseDecomposition md ( dg . digraph () );
  MorseGraph mg ( dg, md );
  if ( command == "json" ) {
    std::cout << mg . stringify ();
  } 
  if ( command == "graphviz" ) {
    std::cout << mg;
  } 
}

void analyze ( int argc, char * argv [] ) {
  std::string command = argv[2];
  if ( command == "morseset" ) {
    std::string ms_index_str = argv[3];
    int64_t ms_index = std::stoll(ms_index_str);
    std::string s = argv[4];
    Parameter p = parse_parameter ( s );
    DomainGraph dg ( p );
    MorseDecomposition md ( dg . digraph () );
    MorseGraph mg ( dg, md );  
    if ( md . recurrent () . size () < ms_index ) {
      std::cout << "Invalid Morse set: There are only " << md . recurrent () . size () << " morse sets.\n";
      return;
    }
    std::stringstream ss;
    ss << "{\"network\":" << network << ",\"parameter\":" << p . stringify () << ",";
    ss << "\"graph\":";
    std::vector<uint64_t> vertices;
    std::unordered_map<uint64_t, uint64_t> reindex;
    for ( uint64_t v : md . recurrent () [ ms_index ] ) {
      reindex [ v ] = vertices . size ();
      vertices . push_back ( v );
    }
    ss << "[";
    bool outerfirst = true;
    for ( uint64_t v : vertices ) {
      if ( outerfirst ) outerfirst = false; else ss << ",";
      ss << "[";
      bool innerfirst = true;
      for ( uint64_t u : dg . digraph () . adjacencies ( v ) ) {
        if ( reindex . count ( u ) ) {
          if ( innerfirst ) innerfirst = false; else ss << ",";
          ss << reindex[u];
        }
      }
      ss << "]";
    }
    ss << "],";
    ss << "\"cells\":";
    ss << "[";
    uint64_t N = vertices . size ();
    Domain dom ( p . network () . domains () );
    outerfirst = true;
    for ( uint64_t i = 0; i < N; ++ i ) {
      if ( outerfirst ) outerfirst = false; else ss << ",";
      dom . setIndex ( vertices [ i ] );
      ss << dom;
    }
    ss << "]}";
    std::cout << ss . str () << "\n";
  } else {
    std::cout << "Unrecognized command: analyze " << command << "\n";
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
  network . load ( filename );
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
      std::cout << network . graphviz ();
      return 0;
    }
    //std::cout << "loading network " << filename << "\n";
    network . load ( filename );
    //std::cout << "setting environment variable...\n";
    argc -= 2; argv += 2;
    if ( argc == 1 ) {
      std::ofstream outfile ( "dsgrn.session" );
      outfile << filename << "\n";
      outfile . close ();
      return 0;
    }
    command = argv[1];
    if ( command == "graphviz" ) {
      std::cout << network . graphviz ();
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

  if ( command == "analyze" ) {
    analyze ( argc, argv );
    return 0;
  }

  std::cout << "Unrecognized command \"" << command << "\"\n";
  return 1;
}
