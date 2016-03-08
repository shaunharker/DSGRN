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


typedef std::string HexCode;
typedef std::string Variable;
typedef std::map<Variable, double> Instance;
typedef std::map<HexCode, Instance> InstanceLookup;

/// Load_CAD_Databases
///   This function loads the CAD databases associated with 
///   each network node of the provided network. 
///   Inputs:   network -- the Network object
///   Objects:  std::vector<json> -- a vector of json objects in 
///             one-to-one correspondence with network nodes
std::vector<json>
Load_CAD_Databases ( Network const& network ) {
  std::cout << "Loading databases.\n";
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
    std::cout << "Reading " << ss.str () << "\n";
    std::ifstream infile (ss.str());
    infile >> J;
    infile.close();
    std::cout << "File read.\n";
  }
  std::cout << "Databases loaded.\n";
  return result;
}

/// Construct_Instance_Lookups
std::vector<InstanceLookup>
Construct_Instance_Lookups ( Network const& network, 
                             std::vector<json> const& CAD_Databases ) {
  uint64_t D = CAD_Databases.size();
  std::vector<InstanceLookup> instancelookups ( D );
  for ( uint64_t d = 0; d < D; ++ d ) {
    for ( auto const& entry : CAD_Databases[d] ) {
      // Obtain hex code from database entry
      std::string hex = entry["Hex"];
      // Obtain instance from database entry
      Instance instance;
      auto json_instance = entry["Instance"];
      for (json::iterator it = json_instance.begin(); it != json_instance.end(); ++it) {
        instance[it.key()] = it.value();
      }
      // Enter instance lookup table entry
      instancelookups[d][hex] = instance;
    }
  }
  return instancelookups;
}

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

/// Gibbs_Sampler
Instance
Gibbs_Sampler ( HexCode const& hex, 
               uint64_t n, 
               uint64_t m,
               Instance const& initial ) {
  // Here is the magic
  Instance result;
  
  uint64_t N = 1 << n;
  const double Inf = std::numeric_limits<double>::infinity();

  // sample
  //   Sample according to exponential distribution conditioned on being
  //   in the interval (min, max)
  auto sample = [](std::pair<double,double> const& pair) {
      double A = std::exp(-pair.first);
      double B = std::exp(-pair.second);
      double mu = distribution(generator);
      return std::log(A - mu *(A-B));
  };
  // Read initial instance into faster data structure
  std::vector<double> L ( n );
  std::vector<double> U ( n );
  std::vector<double> T ( m );
  for ( auto const& pair : initial ) {
    uint64_t i = std::stoll(pair.first.substr(2,pair.first.size()-3));
    switch ( pair.first[0] ) {
      case 'L':
        L[i-1] = pair.second;
        break;
      case 'U':
        U[i-1] = pair.second;
        break;
      case 'T':
        T[i-1] = pair.second;
        break;
    }
  }
  // Initialize "computational tableau" to enable rapid iterations
  // sums[j][k] gives the sum of the terms of the kth factor in the jth L/U formula
  // products[j] gives the product of factors in the jth L/U formula
  // which_factor[i] tells us which factor the ith L (or U) variable is in
  // lower[j] tells us which threshold is the greatest lower bound of the jth L/U formula. m indicates no lower bound
  // upper[j] tells us which threshold is the least upper bound  of the jth L/U formula. m+1 indicates no upper bound
  // we set T[m] = - \infty and T[m+1] = \infty 
  //  a * (b + x)
  std::vector<std::vector<double>> sums;
  std::vector<double> products;
  std::vector<uint64_t> which_factor;
  std::vector<double> cofactor;
  std::vector<double> cosum;

  // TODO, get those sabove ducks in a row

  typedef std::pair<double,double> Interval;

  auto scan = [&](Interval & interval, uint64_t k, double var) {
    double & min = interval.first;
    double & max = interval.second;
    for ( uint64_t j = 0; j < N; ++ j ) {
      cofactor[j] = products[j]/sums[j][k];
      cosum[j] = sums[j][k] - var;
      min = std::max ( min, T[lower[j]]/cofactor[j] - cosum[j]);
      max = std::min ( max, T[upper[j]]/cofactor[j] - cosum[j]);
    }
  }

  auto fix = [&](uint64_t k, double var) {
    for ( uint64_t j = 0; j < N; ++ j ) {
      sums[j][k] = cosum[j] + var;
      products[j] = cofactor[j] * sums[j][k];
    }
  }

  for ( uint64_t burn_in = 0; burn_in < 10; ++ burn_in ) {
    // Update L's
    for ( uint64_t i = 0; i < n; ++ i ) {
      uint64_t k = which_factor[i];
      Interval interval = {0, U[i]};
      scan(interval, k, L[i]);
      L[i] = sample(interval);
      fix(k, L[i]);
    }
    // Update U's
    for ( uint64_t i = 0; i < n; ++ i ) {
      uint64_t k = which_factor[i];
      Interval interval = {L[i], Inf};
      scan(interval, k, U[i]);
      U[i] = sample(interval);
      fix(k, U[i]);
    }
    // Update T's
    for ( uint64_t i = 0; i < m; ++ i ) {
      Interval interval;
      double & min = interval.first;
      double & max = interval.second;
      min = ( i == 0 ) ? 0 : T[i-1];
      max = ( i == (m-1) ) ? Inf : T[i+1];
      for ( uint64_t j = 0; j < N; ++ j ) {
        if ( lower[j] == i ) max = std::min(max,products[j]);
        if ( upper[j] == i ) min = std::max(min,products[j]);
      }
      T[i] = sample(interval);
    }
  }
  return initial;
}

/// Name_Parameters
///   Given a network, a parameter node, a chosen instances for each network node
///   determine the parameter names corresponding to each parameter in the instances
///   and construct a Instance suitable for export
Instance
Name_Parameters ( Network const& network,
                  Parameter const& p,
                  std::vector<Instance> const& instances ) {
  uint64_t D = network . size ();
  std::vector<OrderParameter> const& order = p . order ();
  Instance result;
  for ( uint64_t d = 0; d < D; ++ d ) {
    Instance const& instance = instances[d];
    std::string const& name = network . name ( d );
    // Handle input parameters (i.e. L and U)
    uint64_t n = network . inputs ( d ) . size ();
    for ( uint64_t i = 0; i < n; ++ i ) {
      uint64_t input = network . inputs(d) [ i ];
      std::string const& input_name = network . name ( input );
      result[ "L[" + input_name + ", " + name + "]" ] = instance . at ("L[" + std::to_string(i+1) + "]");
      result[ "U[" + input_name + ", " + name + "]" ] = instance . at ("U[" + std::to_string(i+1) + "]");
    }
    // Handle output parameter (i.e. T)
    uint64_t m = network . outputs ( d ) . size ();
    for ( uint64_t i = 0; i < m; ++ i ) {
      uint64_t output = network . outputs(d) [ order[d](i) ];
      std::string const& output_name = network . name ( output );
      result[ "T[" + name + ", " + output_name + "]" ] = instance . at ("T[" + std::to_string(i+1) + "]");
    }
  }
  return result;
}


/// main
///   Entry point for program
///   Does the following:
///   (1) Check input arguments; if not satisfactory return help message
///   (2) Load the network and the associated CAD database files

int main ( int argc, char * argv [] ) {
  // Check arguments
  if ( argc != 2 ) {
    std::cout << "Received " << argc - 1 << " command line arguments, expected 1. Displaying help message.\n";
    std::cout << help_message << "\n";
    return 1;
  }
  Network network ( argv[1] );
  uint64_t D = network . size ();
  ParameterGraph pg ( network );
  // Load the CAD Databases
  std::vector<json> CAD_Databases = 
    Load_CAD_Databases ( network );
  // Build Instance Lookup tables from the CAD Databases
  std::vector<InstanceLookup> Instance_Lookups = 
    Construct_Instance_Lookups ( network, CAD_Databases );
  // Main loop
  while ( 1 ) {
    // Read from standard input
    uint64_t pi;
    std::cin >> pi;
    // If no standard input to read, break loop
    if ( not std::cin . good () ) break;
    // Compute the parameter associated with the parameter index pi
    Parameter p = pg . parameter ( pi );
    std::vector<LogicParameter> const& logic = p . logic ();
    // Loop through network nodes and extract a sample
    std::vector<Instance> instances;
    for ( uint64_t d = 0; d < D; ++ d ) {
      // Obtain hex code
      HexCode const& hex = logic[d].hex();
      // Obtain initial instance to seed Gibbs sampling with
      Instance const& instance = Instance_Lookups[d][hex];
      // Perform Gibbs sampling
      Instance sampled = Gibbs_Sampler ( hex, network.inputs(d).size(), 
                                         network.outputs(d).size(), instance );
      // Record parameters for network node
      instances . push_back ( sampled );
    }
    // Combine instances into single instance with named parameters
    Instance named_parameters = Name_Parameters ( network, p, instances );
    // Output to standard output
    std::cout << "{";
    bool first = true;
    for ( auto const& pair : named_parameters ) {
      if ( first ) first = false; else std::cout << ", ";
      std::cout << "\"" << pair.first << "\" : " << pair.second;
    }
    std::cout << "}\n";
  }
  return 0;
}
