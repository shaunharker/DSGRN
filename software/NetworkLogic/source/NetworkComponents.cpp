#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#include "FactorGraph.h"
#include "MonotonicMap.h"

/// Partitions

typedef std::vector<int64_t> Partition;

std::ostream & operator << ( std::ostream & stream, Partition const& p ) {
  for ( uint64_t i = 0; i < p . size (); ++ i ) {
    stream << p[i];
    if ( i  < p . size () - 1 ) stream << " ";
  }
  return stream;
}

std::vector<Partition> compute_partitions ( int n ) {
  std::vector<Partition> result;
  result . push_back ( Partition (1, n) );
  for ( int k = 1; k < n; ++ k ) {
    std::vector<Partition> subresult = compute_partitions ( n - k );
    for ( Partition part : subresult ) {
      if ( part . back () > k ) continue;
      part . push_back ( k );
      result . push_back ( part );
    }
  }
  return result;
}

/// Components

struct Component {
  int n;
  int m;
  Partition logic;  
};

std::vector<Component> 
list_components ( void ) {
  std::vector<Component> result;
  for ( int n = 1; n < 6; ++ n ) {
    for ( int m = 1; m < 6; ++ m ) {
      if ( n == 4 && m > 4 ) continue;
      if ( n == 5 && m > 2 ) continue;
      std::vector<Partition> partitions = compute_partitions ( n );
      Component c;
      for ( Partition const& logic : partitions ) {
        c . n = n;
        c . m = m;
        c . logic = logic;
        result . push_back ( c );
      }
    }
  }
  return result;
}

/// Parameter Graph

FactorGraph parametergraph ( int n, int m, Partition const& logic ) {
  MonotonicMap start ( n, m, logic );
  FactorGraph graph;
  graph . construct ( start );
  return graph;
}

FactorGraph parametergraph ( Component const& c ) {
  return parametergraph (c.n,c.m,c.logic);
}  

void display_sizes ( void ) {
  std::vector<Component> components = list_components ();
  for ( Component const& c : components ) {
    try {
      uint64_t nodesize = parametergraph ( c.n, c.m, c.logic ) . size ();
      std::cout << "\"" << c.n << " " << c.m << " " << c.logic << "\" : " << nodesize << ",\n";        
    } catch ( ... ) {}
  }
}

void display_code_test ( int n, int m ) {
  // assume all sum logic for starters
  Partition logic ( 1, n );
  FactorGraph graph = parametergraph ( n, m, logic );
  uint64_t N = graph . size ();
  std::vector<std::string> codes;
  for ( uint64_t i = 0; i < N; ++ i ) { 
    codes . push_back ( graph . vertices [ i ] . hex () );
  }
  std::sort ( codes.begin(), codes.end() );
  for ( uint64_t i = 0; i < N; ++ i ) { 
    std::cout << codes[i] << "\n";
  } 
  std::ofstream outfile ("test.txt");
  for ( uint64_t i = 0; i < N; ++ i ) { 
    outfile << codes[i] << "\n";
  }
  outfile.close();
}

void compute_code ( int n, int m, Partition const& logic ) {
 try {
    FactorGraph graph = parametergraph ( n, m, logic );
    uint64_t N = graph . size ();
    std::vector<std::string> codes;
    for ( uint64_t i = 0; i < N; ++ i ) { 
      codes . push_back ( graph . vertices [ i ] . hex () );
    }
    std::sort ( codes.begin(), codes.end() );
    std::stringstream ss;
    ss << n << " " << m << " " << logic << ".dat";
    std::string filename = ss . str ();
    std::replace ( filename.begin(), filename.end(), ' ', '_' );
    std::ofstream outfile ( filename );
    for ( uint64_t i = 0; i < N; ++ i ) { 
      outfile << codes[i] << "\n";
    }
    outfile . close ();
  } catch ( ... ) {}
}

void compute_code ( Component const& c ) {
  compute_code ( c.n, c.m, c.logic );
}

void compute_all_codes ( void ) {
  std::vector<Component> components = list_components ();
  for ( Component const& c : components ) {
    compute_code ( c );
  }
}



int main ( int argc, char * argv [] ) {
  compute_all_codes ();
  return 0;
  if ( argc < 2 ) {
    display_sizes ();
    return 0;
  }
  display_code_test ( atoi(argv[1]), atoi(argv[2]) );
  return 0;
}
