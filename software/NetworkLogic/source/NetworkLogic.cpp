#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>

#include "FactorGraph.h"
#include "MonotonicMap.h"

//#define ESSENTIAL

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

struct Component {
  int n;
  int m;
  Partition logic;  
};

std::vector<Component> 
list_components ( int N ) {
  std::vector<Component> result;
  for ( int n = 1; n < N; ++ n ) {
    for ( int m = 1; m < 8; ++ m ) {
      if ( n == 3 && m > 5 ) continue;
      if ( n == 4 && m > 2 ) continue;
      if ( n == 5 && m > 1 ) continue;
      std::vector<Partition> partitions = compute_partitions ( n );
      Component c;
      for ( Partition const& logic : partitions ) {
        c . n = n;
        c . m = m;
        c . logic = logic;
        result . push_back ( c );
        //std::cout << n << " " << m << " " << logic << "\n";
      }
    }
  }
  return result;
}

FactorGraph parametergraph ( int n, int m, Partition const& logic ) {
  MonotonicMap start ( n, m, logic );
  FactorGraph graph;
  graph . construct ( start );
  return graph;
}

void compute_code ( int n, int m, Partition const& logic ) {
 try {
    //std::cout << n << " " << m << " " << logic << "\n";

    FactorGraph graph = parametergraph ( n, m, logic );
    uint64_t N = graph . size ();
    std::vector<std::string> codes;
    std::vector<std::string> essentialcodes;

    for ( uint64_t i = 0; i < N; ++ i ) { 
      if ( graph . vertices [ i ] . essential () ) { 
        essentialcodes . push_back ( graph . vertices [ i ] . hex () );
      }
      codes . push_back ( graph . vertices [ i ] . hex () );
    }
    std::sort ( codes.begin(), codes.end() );
    std::sort ( essentialcodes.begin(), essentialcodes.end() );

    {
      std::stringstream ss;
      ss << n << " " << m << " " << logic << ".dat";
      std::string filename = ss . str ();
      std::replace ( filename.begin(), filename.end(), ' ', '_' );
      std::ofstream outfile ( filename );
      for ( uint64_t i = 0; i < codes . size (); ++ i ) { 
        outfile << codes[i] << "\n";
      }
      outfile . close ();
    }
    {
      std::stringstream ss;
      ss << n << " " << m << " " << logic << " " << "E.dat";
      std::string filename = ss . str ();
      std::replace ( filename.begin(), filename.end(), ' ', '_' );
      std::ofstream outfile ( filename );
      for ( uint64_t i = 0; i < essentialcodes . size (); ++ i ) { 
        outfile << essentialcodes[i] << "\n";
      }
      outfile . close ();
    }
  } catch ( ... ) {
    //std::cout << "unable to compute for " << n << " " << m << " " << logic << "\n";
  }


}

void compute_code ( Component const& c ) {
  compute_code ( c.n, c.m, c.logic );
}

void compute_all_codes ( int N ) {
  std::vector<Component> components = list_components ( N );
  for ( Component const& c : components ) {
    compute_code ( c );
  }
}

int main ( int argc, char * argv [] ) {
  uint64_t N = 6;
  if ( argc == 3 ) {
    Component c;
    c . n = std::stoll(argv[1]);
    c . m = std::stoll(argv[2]);
    c . logic = std::vector<int64_t> ( c.n, 1 );
    compute_code ( c );
    return 0;
  }
  if ( argc == 2 ) N = std::stoll(argv[1]);
  compute_all_codes ( N );
  return 0;
}
