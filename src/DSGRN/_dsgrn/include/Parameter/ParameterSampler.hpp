/// ParameterSampler.h
/// Shaun Harker
/// 2018-10-31
/// MIT LICENSE

#include <iostream>
#include <random>

#include "ParameterSampler.h"

inline
ParameterSampler::ParameterSampler (){}

inline
ParameterSampler::ParameterSampler
  (Network network_arg)
{
  assign(network_arg);
}


inline auto
ParameterSampler::assign
  (Network network_arg)
  ->
  void
{
  network = network_arg;
  distribution = std::uniform_real_distribution<double>(0.0,1.0);

  //std::cout << "Loading databases.\n";
  std::vector<json> CAD_Databases;
  // Obtain folder path containing CAD databases
  std::string path = configuration() -> get_path() + "/CAD";

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
    CAD_Databases . push_back ( json () );
    json & J = CAD_Databases . back ();
    // Load the file into the json object
    std::cout << "Reading " << ss.str () << "\n";
    std::ifstream infile (ss.str());
    if ( not infile.good() ) {
      throw std::runtime_error("Missing CAD database " + ss.str() );
    }
    infile >> J;
    infile.close();
    //std::cout << "File read.\n";
  }
  //std::cout << "Databases loaded.\n";

  instancelookups.resize(D);
  for ( uint64_t d = 0; d < D; ++ d ) {
    for ( auto const& entry : CAD_Databases[d] ) {
      // Obtain hex code from database entry
      std::string hex = entry["Hex"];
      // Obtain instance from database entry
      Instance instance;
      auto json_instance = entry["Instance"];
      for (json::iterator it = json_instance.begin();
           it != json_instance.end(); 
           ++it) 
      {
        instance[it.key()] = it.value();
      }
      // Enter instance lookup table entry
      instancelookups[d][hex] = instance;
    }
  }
}

inline auto
ParameterSampler::Gibbs_Sampler
  (ParameterSampler::HexCode const& hex, 
   uint64_t n, 
   uint64_t m,
   std::vector<std::vector<uint64_t>> logic,
   ParameterSampler::Instance const& initial ) const
  ->
  ParameterSampler::Instance
{
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

  // std::cout << "\nGibbsSampler\n";
  // std::cout << "n = " << n << "\n";
  // std::cout << "m = " << n << "\n";
  // std::cout << "N = " << N << "\n";
  // std::cout << "K = " << K << "\n";

  const double Inf = std::numeric_limits<double>::infinity();

  // expsample
  //   Sample according to exponential distribution conditioned on being
  //   in the interval pair = (min, max)
  auto expsample = [this](std::pair<double,double> const& pair) {
      double A = std::exp(-pair.first);
      double B = std::exp(-pair.second);
      double mu = distribution(generator);
      //std::cout << "expsample([" << pair.first << ", " << pair.second << "]) = " << -std::log(A - mu *(A-B)) << "\n";
      double result = -std::log(A - mu *(A-B));
      // if ( result < pair.first || result > pair.second ) {
      //   throw std::logic_error("expsample did not return a value within bounds");
      // }
      return result;
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
  // std::cout << "Compute which_factor (maps variable index to formula factor).\n";
  which_factor . resize ( n );
  {
    uint64_t i = 0;
    for ( uint64_t k = 0; k < K; ++ k ) {
      // std::cout << "  Inspecting factor " << k << "\n";
      uint64_t num_terms = logic [ k ] . size ();
      // std::cout << "    There are " << num_terms << " summands in this factor\n";
      uint64_t start = i;
      uint64_t stop = i + num_terms;
      for ( uint64_t j = start; j < stop; ++ j ) {
        // std::cout << "  Assigning variable " << j << " to factor " << k << "\n";
        // std::cout << "  e.g. which_factor[" << j << "] <- " << k << "\n";
        which_factor[j] = k;
      }
      i = stop;
    }
  }

  // Compute "sums"
  // std::cout << "Compute sums.\n";
  sums . resize ( N, std::vector<double>(K, 0.0));;
  for ( uint64_t j = 0; j < N; ++ j ) {
    uint64_t bit = 1;
    for ( uint64_t i = 0; i < n; ++ i ) {
      // std::cout << "  variable " << i << " is in factor " << which_factor[i] << "\n";
      sums[j][which_factor[i]] += ( j & bit ) ? U[i] : L[i];
      bit <<= 1;
      // std::cout << "  sums[" << j << "][" << which_factor[i] << "] = " << sums[j][which_factor[i]] << "\n";
    }
  }

  // Compute "products"
  // std::cout << "Compute products.\n";
  products . resize ( N, 1.0 );
  for ( uint64_t j = 0; j < N; ++ j ) {
    // std::cout << "  products[" << j << "] = " << products[j] << "\n";
    for ( uint64_t k = 0; k < K; ++ k ) {
      // std::cout << "    update via factor " << k << " with sums[" << j << "][" << k << "] = " << sums[j][k] << "\n";
      products[j] *= sums[j][k]; 
      // std::cout << "    products[" << j << "] <- " << products[j] << "\n";
    }
    // std::cout << "  products[ " << j << "] <- " << products[j] << "\n";
  }

  // Compute "lower" and "upper"
  lower . resize ( N );
  upper . resize ( N );
  LogicParameter lp ( n, m, hex );
  // std::cout << "Scanning lower and upper threshold indices for each equation.\n";
  for ( uint64_t j = 0; j < N; ++ j ) {
    uint64_t bin = lp . bin ( j );
    lower[j] = (bin == 0) ? m+1 : bin - 1;
    upper[j] = bin;
    // std::cout << " inequality " << j << " : bin = " << bin << " lower = " << lower[j]+1 << " and upper = " << upper[j]+1 << "\n";
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
    // std::cout << "Scanning with initial interval (" << min << ", " << max << "), factor index = " 
              // << k << ", mask = " << mask << ", bit = " << bit << ", var = " << var << "\n";
    for ( uint64_t j = 0; j < N; ++ j ) {
      if ( (j & mask) != bit ) continue; 
      // std::cout << "  inequality " << j << "\n";
      cofactor[j] = products[j]/sums[j][k];
      cosum[j] = sums[j][k] - var;
      // std::cout << "    Lower threshold = " << T[lower[j]] << "\n";
      // std::cout << "    Upper threshold = " << T[upper[j]] << "\n";
      // std::cout << "    cofactor["<<j<<"] = " << cofactor[j] << "\n";
      // std::cout << "    cosum["<<j<<"] = " << cosum[j] << "\n";
      // std::cout << "    products["<<j<<"] = " << products[j] << "\n";
      // std::cout << "    sums["<<j<<","<<k<<"] = " << sums[j][k] << "\n";
      // std::cout << "    var = " << var << "\n";
      // std::cout << "    Intersecting with (" << T[lower[j]]/cofactor[j] - cosum[j] << ", " << T[upper[j]]/cofactor[j] - cosum[j] << ")\n";
      min = std::max ( min, T[lower[j]]/cofactor[j] - cosum[j]);
      max = std::min ( max, T[upper[j]]/cofactor[j] - cosum[j]);
    }
    // std::cout << "  returning with interval (" << min << ", " << max << ")\n";
  };
  auto fix = [&](uint64_t k, uint64_t mask, uint64_t bit, double var) {
    for ( uint64_t j = 0; j < N; ++ j ) {
      if ( (j & mask) != bit ) continue; 
      // std::cout << "fix: inequality " << j << "\n";
      sums[j][k] = cosum[j] + var;
      products[j] = cofactor[j] * sums[j][k];
      // std::cout << "  products["<<j<<"] = " << products[j] << "\n";
      // std::cout << "  sums["<<j<<","<<k<<"] = " << sums[j][k] << "\n";
    }
  };
  int burn_in_limit = 10;
  for ( uint64_t burn_in = 0; burn_in < burn_in_limit; ++ burn_in ) {
    // std::cout << "===== round = " << burn_in << "\n";

    // // j indices LUT formulas, of which there are N := 2^n of. 
    // for ( uint64_t j = 0; j < N; ++ j ) {
    //   std::cout << "j = " << j << "\n";
    //   std::cout << "  lower threshold printindex for ineq j = " << lower[j]+1 << "\n";
    //   std::cout << "  upper threshold printindex for ineq j = " << upper[j]+1 << "\n";

    //   std::cout << "  T["<<lower[j] + 1 << "] = " << T[lower[j]] << " ";
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
      // std::cout << "L[" << i+1 << "] update (current value is " << L[i] << ")\n";
      // std::cout << "  Initial interval is 0 < L[" << i+1 << "] < U[" << i+1 << "] which gives (" << 0 << ", " << U[i] << ")\n";
      scan(interval, k, 1 << i, 0, L[i]);
      // std::cout << "  Fixing all other variables gives L[" << i+1 << "] in (" << interval.first << ", " << interval.second << ")\n";
      L[i] = expsample(interval);
      // std::cout << "  Updating L[" << i+1 << "] <- " << L[i] << "\n";
      fix(k, 1 << i, 0, L[i]);
    }
    // Update U's
    for ( uint64_t i = 0; i < n; ++ i ) {
      uint64_t k = which_factor[i];
      Interval interval = {L[i], Inf};
      // std::cout << "U[" << i+1 << "] update (current value is " << U[i] << ")\n";
      // std::cout << "  Initial interval is L[" << i+1 << "] < U[" << i+1 << "] < inf which gives (" << U[i] << ", inf )\n";
      scan(interval, k, 1 << i, 1 << i, U[i]);
      // std::cout << "  Fixing all other variables gives gives U[" << i+1 << "]  (" << interval.first << ", " << interval.second << ")\n";
      U[i] = expsample(interval);
      // std::cout << "  Updating U[" << i+1 << "] <- " << U[i] << "\n";
      fix(k, 1 << i, 1 << i, U[i]);
    }
    // Update T's
    for ( uint64_t i = 0; i < m; ++ i ) {
      Interval interval;
      double & min = interval.first;
      double & max = interval.second;
      min = ( i == 0 ) ? 0.0 : T[i-1];
      max = ( i == (m-1) ) ? Inf : T[i+1];
      // std::cout << "T[" << (i+1) << "] (currently = " << T[i] << ")\n";
      // std::cout << "  Initial interval is  ( T[" << i << "] = " << min << ", T[" << i+2 << "] = " << max << ")\n";
      for ( uint64_t j = 0; j < N; ++ j ) {
        if ( lower[j] == i ) { 
          // std::cout << "  Intersecting with (0, " << products[j] << ") due to inequality " << j << "\n";
          max = std::min(max,products[j]);
        }
        if ( upper[j] == i ) { 
          // std::cout << "  Intersecting with (" << products[j] << ", inf) due to inequality " << j << "\n";
          min = std::max(min,products[j]);
        }
      }
      // std::cout << "  Constraints give interval is  [T[" << i << "] in (" << min << ", " << max << ")\n";
      T[i] = expsample(interval);
      // std::cout << "  T[" << (i+1) << "] <- " << T[i] << "\n";
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
  // std::cout << "DEBUG: Check this. Hex = " << hex << " and " << InstanceToString(result) << "\n";
  return result;
}

inline auto
ParameterSampler::Name_Parameters 
  (Parameter const& p,
   std::vector<Instance> const& instances) const
  ->
  ParameterSampler::Instance 
{
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

inline auto
ParameterSampler::InstanceToString
  (ParameterSampler::Instance const& instance ) const 
  ->
  std::string 
{
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

inline auto
ParameterSampler::sample
  (Parameter p) const
  ->
  std::string
{
  uint64_t D = network . size ();
  std::vector<LogicParameter> const& logic = p . logic ();
  // Loop through network nodes and extract a sample
  std::vector<Instance> instances;
  for ( uint64_t d = 0; d < D; ++ d ) {
    // Obtain hex code
    HexCode const& hex = logic[d].hex();
    // Obtain initial instance to seed Gibbs sampling with
    Instance const& instance = instancelookups[d].at(hex);
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
  Instance named_parameters = Name_Parameters ( p, instances );
  // Output
  std::stringstream ss;
  ss << "{\"Parameter\":" << InstanceToString(named_parameters) << "}";
  return ss . str ();
}
