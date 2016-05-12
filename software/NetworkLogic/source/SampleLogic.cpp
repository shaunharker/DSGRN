#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <random>
#include <iomanip>
#include <map>
#include <algorithm>

std::string helpstring =
  " SampleLogic\n"
  "   This program samples parameters for network nodes \n"
  "   and determines the corresponding logic parameters, \n"
  "   which are outputted as hexcodes along with probability \n"
  "   measure estimates\n"
  " Usage: SampleLogic n m p1 p2 ... pk N \n"
  "    where n is number of network node in-edges\n"
  "          m is number of network node out-edges\n"
  "          pi is the number of variables in the ith factor, so p1+p2+...+pk = n\n"
  "          N is a number of samples to obtain and test.\n";

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0,1.0);

/// LogicSampler
///   Inputs: n (number of network node inputs)
///           m (number of network nodes outputs)
///           logic ( array of integers encoding product-of-sums algebra, e.g.
///            a*b*(c+d) <--> [1, 1, 2]
///   Output: a hex code representing the logic parameter (see DSGRN documentation)
std::string
LogicSampler ( uint64_t n, 
               uint64_t m,
               std::vector<uint64_t> const& logic ) {
  uint64_t N = 1 << n;
  uint64_t K = logic . size ();

  // Obtain sample
  std::vector<double> L ( n );
  std::vector<double> U ( n );
  std::vector<double> T ( m );
  auto sample = [&](void) {return -std::log(distribution(generator));};
  for ( uint64_t i = 0; i < n; ++ i ) {
    L [ i ] = sample ();
    U [ i ] = sample ();
    if ( U[i] < L[i] ) std::swap ( L[i], U[i] );
  }
  for ( uint64_t i = 0; i < m; ++ i ) {
    T [ i ] = sample ();
  }
  std::sort ( T . begin (), T . end () );

  // Compute 2^n L/U formula values
  std::vector<double> values ( N );
  for ( uint64_t i = 0; i < N; ++ i ) {
    double value = 1.0;
    uint64_t offset = 0;
    for ( uint64_t k = 0; k < K; ++ k ) {
      double summand = 0.0;
      for ( uint64_t in = 0; in < logic[k]; ++ in ) {
        uint64_t var = in + offset;
        double activation_level = ( i & ( 1 << var ) ) ? U[var] : L[var];
        summand += activation_level;
      }
      offset += logic[k];
      value *= summand;
    }
    values [ i ] = value;
  }

  // Compare 2^n L/U formula values to m thresholds to create binary code
  uint64_t hex_code = 0;
  uint64_t bit = 1;
  for ( uint64_t i = 0; i < N; ++ i ) {
    for ( uint64_t j = 0; j < m; ++ j ) {
      if ( values[i] > T[j] ) hex_code |= bit;
      bit <<= 1;
    }
  }

  // Output hex code
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(N*m/4) << std::uppercase << std::hex << hex_code;
  return ss . str ();
}

/// main
///   Comman
int main ( int argc, char * argv [] ) {
  // Load partition from command line arguments
  uint64_t n;
  uint64_t m;
  std::vector<uint64_t> logic;
  if ( argc < 3 ) { 
    std::cout << helpstring;
    return 1;
  }
  n = std::stoll(argv[1]);
  m = std::stoll(argv[2]);
  int argnum = 3;
  uint64_t x = 0;
  while ( x != n ) {
    if ( argc == argnum || x > n ) {
      std::cout << "Invalid command line arguments. Invoke without arguments for help.\n";
      return 1;
    }
    int factor_size = std::stoll(argv[argnum]);
    logic . push_back ( factor_size );
    x += factor_size; 
    ++ argnum;
  }
  
  // Load number of samples from command line arguments
  if ( argc == argnum ) {
    std::cout << "Invalid command line arguments. Invoke without arguments for help.\n";
    return 1;
  }
  uint64_t N = std::stoll ( argv[argnum] );

  // Sample
  std::map<std::string, uint64_t> hex_codes;
  for ( uint64_t i = 0; i < N; ++ i ) {
    //std::cout << "Top of loop. i = " << i << "\n";
    std::string hex_code = LogicSampler ( n, m, logic );
    hex_codes[hex_code] = hex_codes[hex_code] + 1;
  }

  // Display results
  for ( auto const& pair : hex_codes ) {
    std::cout << pair.first << " " << pair.second << "\n";
  }

  // Return
  return 0;
}
