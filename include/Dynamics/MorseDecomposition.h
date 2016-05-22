/// MorseDecomposition.h
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_MORSEDECOMPOSITION_H
#define DSGRN_MORSEDECOMPOSITION_H

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "common.h"

#include "Graph/Digraph.h"
#include "Graph/Components.h"
#include "Graph/Poset.h"

struct MorseDecomposition_;

class MorseDecomposition {
public:
  /// constructor
  MorseDecomposition ( void );

  /// MorseDecomposition
  ///   Create a Morse Decomposition given
  ///   a digraph.
  MorseDecomposition ( Digraph const& digraph );

  /// MorseDecomposition
  ///   Create a Morse Decomposition given
  ///   a digraph and its strong components
  ///   (This method is provided in case 
  ///    strong components are already computed.)
  MorseDecomposition ( Digraph const& digraph, 
                       Components const& components );

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph.
  void
  assign ( Digraph const& digraph );

  /// assign
  ///   Create a Morse Decomposition given
  ///   a digraph and its strong components
  ///   (This method is provided in case 
  ///    strong components already computed.)
  void
  assign ( Digraph const& digraph, 
           Components const& components );

  /// poset 
  ///   Return the underlying partial order
  Poset const
  poset ( void ) const;

  /// components 
  ///   Return all components 
  Components const
  components ( void ) const;

  /// components 
  ///   Return recurrent components 
  Components::ComponentContainer const
  recurrent ( void ) const;

  /// graphviz
  ///   Return a graphviz representation
  std::string
  graphviz ( void ) const;

  /// operator <<
  ///   Stream information to graphviz format
  friend std::ostream& operator << ( std::ostream& stream, MorseDecomposition const& md );

private:
  std::shared_ptr<MorseDecomposition_> data_;
  /// _canonicalize
  ///   Renumber the vertices in a "canonical" fashion
  ///   given the available information. (Note: not every
  ///   isomorphic graph has the same canonicalization.)
  void _canonicalize ( void );
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & data_;
  }
};

struct MorseDecomposition_ {
  Components components_;
  Poset poset_;
  /// serialize
  ///   For use with BOOST Serialization library,
  ///   which is used by the cluster-delegator MPI package
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & components_;
    ar & poset_;
  }
};

#endif
