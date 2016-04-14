/// TestPoset.cpp
/// Arnaud Goullet
/// 2016-01-26

#include "common.h"
#include "DSGRN.h"

int main ( int argc, char * argv [] ) {
  try {
    // Create two posets that have identical structure
    // the nodes have identical annotations but different numbering
    // From the posets and annotations, create two morsegraphs
    // the constructor will call the canonicalize method.
    // The two morsegraphs should be identical (numbering and annotations)
    Digraph digraph1, digraph2;
    Poset poset1, poset2;
    std::unordered_map<uint64_t, Annotation> annotations1, annotations2;
    // Declaration of the first poset
    uint64_t n0 = digraph1 . add_vertex ();
    uint64_t n1 = digraph1 . add_vertex ();
    uint64_t n2 = digraph1 . add_vertex ();
    uint64_t n3 = digraph1 . add_vertex ();
    uint64_t n4 = digraph1 . add_vertex ();
    uint64_t n5 = digraph1 . add_vertex ();
    uint64_t n6 = digraph1 . add_vertex ();
    digraph1 . add_edge ( n0, n1 );
    digraph1 . add_edge ( n0, n6 );
    digraph1 . add_edge ( n1, n2 );
    digraph1 . add_edge ( n1, n3 );
    digraph1 . add_edge ( n3, n4 );
    digraph1 . add_edge ( n3, n5 );
    digraph1 . finalize ();
    // Define the annotations
    for ( uint64_t i=0; i<digraph1.size(); ++i ) {
      std::stringstream ss;
      ss << i;
      Annotation newAnnotation;
      newAnnotation . append ( ss.str() );
      annotations1 [ i ] = newAnnotation;
    }
    // Declaration of the second poset
    uint64_t nn0 = digraph2 . add_vertex ();
    uint64_t nn1 = digraph2 . add_vertex ();
    uint64_t nn2 = digraph2 . add_vertex ();
    uint64_t nn3 = digraph2 . add_vertex ();
    uint64_t nn4 = digraph2 . add_vertex ();
    uint64_t nn5 = digraph2 . add_vertex ();
    uint64_t nn6 = digraph2 . add_vertex ();
    digraph2 . add_edge ( nn0, nn1 );
    digraph2 . add_edge ( nn0, nn3 );
    digraph2 . add_edge ( nn1, nn6 );
    digraph2 . add_edge ( nn1, nn2 );
    digraph2 . add_edge ( nn2, nn4 );
    digraph2 . add_edge ( nn2, nn5 );
    digraph2 . finalize ();
    annotations2 = annotations1;
    // Fix 3 nodes, because we swapped 3 nodes numbers from digraph1
    Annotation an2, an3, an6;
    an2 . append ( "2" );
    an3 . append ( "3" );
    an6 . append ( "6" );
    annotations2 [ 6 ] = an2;
    annotations2 [ 2 ] = an3;
    annotations2 [ 3 ] = an6;
    /// Apply transitive closure and reduction
    poset1 . assign ( digraph1 );
    poset2 . assign ( digraph2 );

    /// output on the screen te two posets and annotations
    std::cout << "Poset1 Digraph\n";
    std::cout << poset1;
    std::cout << "Poset1 Annotations\n";
    for ( uint64_t i=0; i<poset1.size(); ++i ) {
      for ( auto u : annotations1[i] ) {
        std::cout << u << " ";
      }
      std::cout << "\n";
    }
    std::cout << "Poset2 Digraph\n";
    std::cout << poset2;
    std::cout << "Poset2 Annotations\n";
    for ( uint64_t i=0; i<poset2.size(); ++i ) {
      for ( auto u : annotations2[i] ) {
        std::cout << u << " ";
      }
      std::cout << "\n";
    }
    // Construct two morsegraphs from the posets and annotations
    // The constructor will call the canonicalize method
    MorseGraph mg1 ( poset1, annotations1 );
    MorseGraph mg2 ( poset2, annotations2 );
    // Output on the screen the two posets and annotations from the morsegraphs
    // they should be identical
    std::cout << "============================\n";
    std::cout << "After Morsegraph Constructor\n";
    std::cout << "============================\n";
    std::cout << "Poset1 Digraph\n";
    std::cout << mg1 . poset ();
    std::cout << "Poset1 Annotations\n";
    for ( uint64_t i = 0; i < poset1.size(); ++i ) {
      for ( auto u : mg1.annotation(i) ) {
        std::cout << u << " ";
      }
      std::cout << "\n";
    }
    std::cout << "Poset2 Digraph\n";
    std::cout << mg2 . poset();
    std::cout << "Poset2 Annotations\n";
    for ( uint64_t i = 0; i < poset2.size(); ++i ) {
      for ( auto u : mg2.annotation(i) ) {
        std::cout << u << " ";
      }
      std::cout << "\n";
    }

  } catch ( std::exception & e ) {
    std::cout << e . what () << "\n";
    return 1;
  }
  return 0;
}
