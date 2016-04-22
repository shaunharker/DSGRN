/// ParameterSampler.cpp
/// Shaun Harker
/// 2016-03-07

#include <iostream>
#include <random>

#include "DSGRN.h"

std::string help_message = 
  "ParameterSampler                                                         \n"
  "    Perform Gibbs sampling to extract a parameter sample corresponding to\n"
  "    a regulatory network and a parameter index.                          \n"
  "  Usage:                                                                 \n"
  "  (1) ParameterSampler <networkspace_file>                               \n"
  "      Reads parameter node indices from standard input and output corresponding samples.\n"
  "  (2) ParameterSampler <networkspace_file> <parameter_index_1> [<parameter_index_2> ...]\n"
  "      Read one or more parameter node indices from command line \n"
  "      and output corresponding samples.                         \n"
  "  Examples:                                                     \n"
  "    cat parameter_index_list.txt | ./bin/ParameterSampler ../../networks/13D_p53.txt \n"
  "    ./bin/ParameterSampler ../../networks/13D_p53.txt 145 \n"
  "    ./bin/ParameterSampler ../../networks/13D_p53.txt 145 2788\n";


typedef std::string HexCode;
typedef std::string Variable;
typedef std::map<Variable, double> Instance;
typedef std::map<HexCode, Instance> InstanceLookup;


/// InstanceToString
///   Make a string from an instance
std::string 
InstanceToString ( Instance const& instance ) {
  std::stringstream ss;
  ss . precision ( std::numeric_limits< double >::max_digits10 );
  ss << "{";
  bool first = true;
  for ( auto const& pair : instance ) {
    if ( first ) first = false; else ss << ", ";
    ss << "\"" << pair.first << "\" : " << pair.second;
  }
  ss << "}";
  return ss . str ();
}

/// Load_CAD_Databases
///   This function loads the CAD databases associated with 
///   each network node of the provided network. 
///   Inputs:   network -- the Network object
///   Objects:  std::vector<json> -- a vector of json objects in 
///             one-to-one correspondence with network nodes
std::vector<json>
Load_CAD_Databases ( Network const& network ) {
  //std::cout << "Loading databases.\n";
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
    //std::cout << "Reading " << ss.str () << "\n";
    std::ifstream infile (ss.str());
    infile >> J;
    infile.close();
    //std::cout << "File read.\n";
  }
  //std::cout << "Databases loaded.\n";
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
                std::vector<std::vector<uint64_t>> logic,
                Instance const& initial ) {
  // The basic idea of the algorithm is as follows.
  // We have 2^n formulas of the form (L[1])(L[2]+U[3]) (for various U/L combinations)
  // In Gibb's sampling, we update one variable at a time, but we need to see
  // what region the inequalities constrain us to. This involves inspecting the 2^n inequalities.
  // In order to accelerate this task, we try to avoid arithmetic computations by cleverly 
  // updating the numbers correspond each sum and each product occuring in the formulas.
  // If we change a single variable, we can update the sum it lives in easily by 
  // adding the new value to the old sum minus the old value. We use the variable "cosum"
  // to store the old sum minus the old variable value. We do a similar trick with "coproduct".
  // This enables us to find the lower and upper bounds enforced by an inequality in the
  // following manner:
  //     c < a*(b+x) < d  --> c/a-b < x < d/a - b
  //     Here a is the "coproduct" and "b" is the "cosum"
  
  uint64_t N = 1 << n;
  uint64_t K = logic . size ();

  const double Inf = std::numeric_limits<double>::infinity();

  // sample
  //   Sample according to exponential distribution conditioned on being
  //   in the interval pair = (min, max)
  auto sample = [](std::pair<double,double> const& pair) {
      double A = std::exp(-pair.first);
      double B = std::exp(-pair.second);
      double mu = distribution(generator);
      //std::cout << "sample([" << pair.first << ", " << pair.second << "]) = " << -std::log(A - mu *(A-B)) << "\n";
      return -std::log(A - mu *(A-B));
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
  //   which_factor[i] : which factor the ith L (or U) variable is in
  //   sums[j][k]      : the sum of the terms of the kth factor in the jth L/U formula
  //   products[j]     : the product of factors in the jth L/U formula
  //   lower[j]        : threshold that is the greatest lower bound of the jth L/U formula. m+1 indicates no lower bound
  //   upper[j]        : threshold that is the least upper bound  of the jth L/U formula. m indicates no upper bound
  //   We set T[m] = Inf and T[m+1] = 0 so T[lower[j]] and T[[upper[j]] reflect lack of lower and upper bounds properly
  std::vector<uint64_t> which_factor;
  std::vector<std::vector<double>> sums;
  std::vector<double> products;
  std::vector<uint64_t> lower;
  std::vector<uint64_t> upper;

  // Compute "which_factor"
  which_factor . resize ( n );
  {
    uint64_t i = 0;
    for ( uint64_t k = 0; k < K; ++ k ) {
      uint64_t num_terms = logic [ k ] . size ();
      uint64_t start = i;
      uint64_t stop = i + num_terms;
      for ( uint64_t i = start; i < stop; ++ i ) {
        which_factor[i] = k;
      }
    }
  }

  // Compute "sums"
  sums . resize ( N, std::vector<double>(K, 0.0));;
  for ( uint64_t j = 0; j < N; ++ j ) {
    uint64_t bit = 1;
    for ( uint64_t i = 0; i < n; ++ i ) {
      sums[j][which_factor[i]] += ( j & bit ) ? U[i] : L[i];
      bit <<= 1;
    }
  }

  // Compute "products"
  products . resize ( N, 1.0 );
  for ( uint64_t j = 0; j < N; ++ j ) {
    for ( uint64_t k = 0; k < K; ++ k ) {
      products[j] *= sums[j][k]; 
    }
  }

  // Compute "lower" and "upper"
  lower . resize ( N );
  upper . resize ( N );
  LogicParameter lp ( n, m, hex );
  for ( uint64_t j = 0; j < N; ++ j ) {
    uint64_t bin = lp . bin ( j );
    lower[j] = (bin == 0) ? (m+1) : bin - 1;
    upper[j] = bin;
  }
  // Add special values to T so T[lower[j]] and T[[upper[j]] give what we want
  T . push_back ( Inf ); // T[m]
  T . push_back ( 0.0 ); // T[m+1]

  // Temporary storage variables
  std::vector<double> cofactor(N);
  std::vector<double> cosum(N);

  typedef std::pair<double,double> Interval;
  // Note: we use a mask/bit arguments to filter out all inequalities where we have L[i] instead of U[i]
  //       or vice-versa as the case requires
  auto scan = [&](Interval & interval, uint64_t k, uint64_t mask, uint64_t bit, double var) {
    double & min = interval.first;
    double & max = interval.second;
    for ( uint64_t j = 0; j < N; ++ j ) {
      if ( (j & mask) != bit ) continue; 
      //std::cout << "Scanning inequality " << j << "\n";
      cofactor[j] = products[j]/sums[j][k];
      cosum[j] = sums[j][k] - var;
      // std::cout << "Lower threshold = " << T[lower[j]] << "\n";
      // std::cout << "Upper threshold = " << T[upper[j]] << "\n";
      // std::cout << "cofactor["<<j<<"] = " << cofactor[j] << "\n";
      // std::cout << "cosum["<<j<<"] = " << cosum[j] << "\n";
      // std::cout << "products["<<j<<"] = " << products[j] << "\n";
      // std::cout << "sums["<<j<<","<<k<<"] = " << sums[j][k] << "\n";
      // std::cout << "var = " << var << "\n";
      min = std::max ( min, T[lower[j]]/cofactor[j] - cosum[j]);
      max = std::min ( max, T[upper[j]]/cofactor[j] - cosum[j]);
    }
  };
  auto fix = [&](uint64_t k, uint64_t mask, uint64_t bit, double var) {
    for ( uint64_t j = 0; j < N; ++ j ) {
      if ( (j & mask) != bit ) continue; 
      sums[j][k] = cosum[j] + var;
      products[j] = cofactor[j] * sums[j][k];
    }
  };
  for ( uint64_t burn_in = 0; burn_in < 10; ++ burn_in ) {
    // std::cout << "===== round = " << burn_in << "\n";

    // for ( uint64_t j = 0; j < N; ++ j ) {
    //   std::cout << "T["<<lower[j] + 1 << "] = " << T[lower[j]] << " ";
    //   for ( uint64_t i = 0; i < n; ++ i ) {
    //     if ( ( j & (1 << i)) == 0 ) {
    //       std::cout << "L[" << i+1 << "] = " << L[i] << " ";
    //     } else {
    //       std::cout << "U[" << i+1 << "] = " << U[i] << " ";
    //     }
    //   }
    //   std::cout << "T["<<upper[j] + 1 << "] = " << T[upper[j]] << "\n";
    // }

    // Update L's
    for ( uint64_t i = 0; i < n; ++ i ) {
      uint64_t k = which_factor[i];
      Interval interval = {0, U[i]};
      //std::cout << "L[" << i << "] computation. interval = {" << 0 << ", " << U[i] << "}\n";
      scan(interval, k, 1 << i, 0, L[i]);
      //std::cout << "L[" << i << "] computation. interval after scan = {" << interval.first << ", " << interval.second << "}\n";
      L[i] = sample(interval);
      //std::cout << "L[" << i << "] = " << L[i] << "\n";
      fix(k, 1 << i, 0, L[i]);
    }
    // Update U's
    for ( uint64_t i = 0; i < n; ++ i ) {
      uint64_t k = which_factor[i];
      Interval interval = {L[i], Inf};
      //std::cout << "U[" << i << "] computation. interval = {" << L[i] << ", " << Inf << "}\n";
      scan(interval, k, 1 << i, 1 << i, U[i]);
      //std::cout << "U[" << i << "] computation. interval after scan = {" << interval.first << ", " << interval.second << "}\n";
      U[i] = sample(interval);
      //std::cout << "U[" << i << "] = " << U[i] << "\n";
      fix(k, 1 << i, 1 << i, U[i]);
    }
    // Update T's
    for ( uint64_t i = 0; i < m; ++ i ) {
      Interval interval;
      double & min = interval.first;
      double & max = interval.second;
      min = ( i == 0 ) ? 0.0 : T[i-1];
      max = ( i == (m-1) ) ? Inf : T[i+1];
      for ( uint64_t j = 0; j < N; ++ j ) {
        if ( lower[j] == i ) max = std::min(max,products[j]);
        if ( upper[j] == i ) min = std::max(min,products[j]);
      }
      T[i] = sample(interval);
    }
  }
  // Create Instance object containing result
  Instance result;
  for ( uint64_t i = 0; i < n; ++ i ) {
    result["L[" + std::to_string(i+1) + "]"] = L[i];
    result["U[" + std::to_string(i+1) + "]"] = U[i];
  }
  for ( uint64_t i = 0; i < m; ++ i ) {
    result["T[" + std::to_string(i+1) + "]"] = T[i];
  }
  //std::cout << "DEBUG: Check this. Hex = " << hex << " and " << InstanceToString(result) << "\n";
  return result;
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


std::string
GetSample ( Network const& network, 
            ParameterGraph const& pg,
            std::vector<InstanceLookup> const& Instance_Lookups, 
            uint64_t pi ) {
  uint64_t D = network . size ();
  // Compute the parameter associated with the parameter index pi
  Parameter p = pg . parameter ( pi );
  std::vector<LogicParameter> const& logic = p . logic ();
  // Loop through network nodes and extract a sample
  std::vector<Instance> instances;
  for ( uint64_t d = 0; d < D; ++ d ) {
    // Obtain hex code
    HexCode const& hex = logic[d].hex();
    // Obtain initial instance to seed Gibbs sampling with
    Instance const& instance = Instance_Lookups[d].at(hex);
    // Perform Gibbs sampling
    Instance sampled = Gibbs_Sampler ( hex, 
                                       network.inputs(d).size(), 
                                       network.outputs(d).size(), 
                                       network.logic(d),
                                       instance );
    // Record parameters for network node
    instances . push_back ( sampled );
  }
  // Combine instances into single instance with named parameters
  Instance named_parameters = Name_Parameters ( network, p, instances );
  // Output to standard output
  std::stringstream ss;
  ss << "{\"ParameterIndex\":" << pi << ",\"Parameter\":" << InstanceToString(named_parameters) << "}";
  return ss . str ();
}
/// main
///   Entry point for program
///   Does the following:
///   (1) Check input arguments; if not satisfactory return help message
///   (2) Load the network and the associated CAD database files

int main ( int argc, char * argv [] ) {
  // Check arguments
  if ( argc == 1 ) {
    std::cout << "Received " << argc - 1 << " command line arguments, expected 1. Displaying help message.\n";
    std::cout << help_message << "\n";
    return 1;
  }
  Network network ( argv[1] );
  ParameterGraph pg ( network );
  // Load the CAD Databases
  std::vector<json> CAD_Databases = 
    Load_CAD_Databases ( network );
  // Build Instance Lookup tables from the CAD Databases
  std::vector<InstanceLookup> Instance_Lookups = 
    Construct_Instance_Lookups ( network, CAD_Databases );
  // Read parameter indices from command else if they are there
  // else read from standard input
  if ( argc > 2 ) {
    for ( uint64_t i = 2; i < argc; ++ i ) {
      uint64_t pi = std::stoll(argv[i]);
      std::cout << GetSample ( network, pg, Instance_Lookups, pi ) << "\n"; 
    }
  } else {
    while ( 1 ) {
      // Read from standard input
      uint64_t pi;
      std::cin >> pi;
      // If no standard input to read, break loop
      if ( not std::cin . good () ) break;
      std::cout << GetSample ( network, pg, Instance_Lookups, pi ) << "\n"; 
    }
  }
  return 0;
}
