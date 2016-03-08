/// ParameterSampler.cpp
/// Shaun Harker
/// 2016-03-07

#include <iostream>

#include "DSGRN.h"

std::string help_message = 
  "ParameterSampler                                                  \n"
  "    Reads parameter node indices from standard input and outputs  \n"
  "    Gibbs sampled parameters to standard output.                  \n"
  "  Usage:                                                          \n"
  "    ParameterSampler <networkspace_file>                         \n"
  "  Example:                                                        \n"
  "    cat parameter_index_list.txt | ./bin/ParameterSampler ../../networks/13D_p53.txt \n";


/// Load_CAD_Databases
///   This function loads the CAD databases associated with 
///   each network node of the provided network. 
///   Inputs:   network -- the Network object
///   Objects:  std::vector<json> -- a vector of json objects in 
///             one-to-one correspondence with network nodes
std::vector<json>
Load_CAD_Databases ( Network const& network ) {
  std::vector<json> result;
  // Obtain folder path containing CAD databases
  std::string path = std::string(INSTALLPREFIX) + "/share/DSGRN/CAD";
  // Load the CAD databases one by one
  uint64_t D = network . size ();
  for ( uint64_t d = 0; d < D; ++ d ) {
    // Construct CAD database file name for network node
    uint64_t n = network . inputs ( d ) . size ();
    uint64_t m = network . outputs ( d ) . size ();
    std::vector<std::vector<uint64_t>> const& logic_struct = network . logic ( d );
    std::stringstream ss;
    ss << path << "/" << n <<  "_" << m;
    for ( auto const& p : logic_struct ) ss <<  "_" << p.size();
    ss << ".json";
    // Make a new json object to store the file into
    result . push_back ( json () );
    json & J = result . back ();
    // Load the file into the json object
    std::ifstream infile (ss.str());
    infile >> J;
    infile.close();
    // BEGIN DEBUG
    // inspect our work to make sure it worked 
    std::cout << J << "\n";
    // END DEBUG
  }
  return result;
}

typedef std::string HexCode;
typedef std::string Variable;
typedef std::map<Variable, double> Instance;

/// Construct_Instance_Lookups
// typedefs of string to make code easier to follow
std::map<HexCode, Instance>
Construct_Instance_Lookups ( Network const& network, 
                             std::vector<json> const& CAD_Databases ) {

}
/// Lookup_Initial_Point
///   Search through databases
Instance
Lookup_Initial_Point ( Network const& network, 
                       Parameter const& p,
                       std::map<HexCode, Instance> const& Instance_Lookups ) {

}

///
Instance
GibbsSampler ( HexCode const& hex, 
               uint64_t n, 
               uint64_t m,
               Instance const& initial ) {
  // Here is the magic
}

/// main
///   Entry point for program
///   Does the following:
///   (1) Check input arguments; if not satisfactory return help message
///   (2) Load the network and the associated CAD database files

int main ( int argc, char * argv [] ) {
  std::cout << "Hello world! (not exactly a Gibbs sampler yet.)\n";
  // Check arguments
  if ( argc != 2 ) {
    std::cout << "Received " << argc - 1 << " command line arguments, expected 1. Displaying help message.\n";
    std::cout << help_message << "\n";
    return 1;
  }
  Network network ( argv[1] );
  ParameterGraph pg ( network );

  std::cout << "ParameterGraph size = " << pg . size () << "\n";
  // Load the CAD Databases
  std::vector<json> CAD_Databases = Load_CAD_Databases ( network );
  // Build Instance Lookup tables from the CAD Databases
  std::map<HexCode, Instance> Instance_Lookups = Construct_Instance_Lookups ( network, CAD_Databases );

  while ( 1 ) {
    // Read from standard input
    uint64_t pi;
    std::cin >> pi;
    // If no standard input to read, break loop
    if ( not std::cin . good () ) break;
    // Compute the parameter associated with the parameter index pi
    Parameter p = pg . parameter ( pi );
    

    // Output to standard output

    // TODO
  }
  return 0;
}
