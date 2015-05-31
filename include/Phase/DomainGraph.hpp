/// DomainGraph.hpp
/// Shaun Harker
/// 2015-05-24

#ifndef DSGRN_DOMAINGRAPH_HPP
#define DSGRN_DOMAINGRAPH_HPP

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "DomainGraph.h"

INLINE_IF_HEADER_ONLY DomainGraph::
DomainGraph ( void ) { 
  data_ . reset ( new DomainGraph_ );
}

INLINE_IF_HEADER_ONLY DomainGraph::
DomainGraph ( Parameter const parameter ) {
  assign ( parameter );
} 

INLINE_IF_HEADER_ONLY void DomainGraph::
assign ( Parameter const parameter ) {
  data_ . reset ( new DomainGraph_ );
  data_ -> parameter_ = parameter;
  int D = parameter . network() . size ();
  uint64_t N = 1;
  std::vector<uint64_t> limits = parameter . network() . domains ();
  for ( int d = 0; d < D; ++ d ) N *= limits [ d ];
  data_ -> digraph_ = Digraph ();
  data_ -> digraph_ . resize ( N );
  std::vector<uint64_t> label_data ( N );
  std::vector<std::pair<uint64_t,uint64_t>> edges;
  for ( Domain dom (limits); dom.isValid(); ++ dom ) {
    uint64_t i = dom . index ();
    if ( parameter . attracting ( dom ) ) {
      edges . push_back ( std::make_pair(i,i) );
      data_ ->digraph_ . add_edge ( i, i );
    }
    for ( int d = 0; d < D; ++ d ) {
      if ( not dom . isMin(d) ) {
        if ( parameter . absorbing ( dom, d, -1 ) ) {
          uint64_t j = dom . left ( d );
          edges . push_back ( std::make_pair(i,j) );
        }
      }
      if ( not dom . isMax(d) ) {
        if ( parameter . absorbing ( dom, d, 1 ) ) {
          uint64_t j = dom . right ( d );
          edges . push_back ( std::make_pair(i,j) );
        }
      }
    }
  }
  // We do not want all edges produced; only the ones that
  // do not have a corresponding back edge. (i.e. when two domains
  // have edges to each other we remove those edges.)
  typedef std::pair<uint64_t,uint64_t> Edge;
  auto compare_edge = [](Edge const& lhs, Edge const& rhs){
    if ( lhs.first < rhs.first ) return true;
    if ( lhs.first > rhs.first ) return false;
    if ( lhs.second < rhs.second ) return true;
    if ( lhs.second > rhs.second ) return false;
    return false;
  };
  uint64_t E = edges . size ();
  std::vector<std::pair<uint64_t,uint64_t>> backedges;
  backedges . reserve ( edges.size () );
  for ( auto const& edge : edges ) backedges . push_back ( Edge(edge.second,edge.first) );
  std::sort ( edges.begin(), edges.end(), compare_edge );
  std::sort ( backedges.begin(), backedges.end(), compare_edge );
  uint64_t i = 0, j = 0; 
  while ( i < edges . size () ) {
    //std::cout << "Considering edge ["<<edges[i].first << " -> " << edges[i].second << "].\n";
    if ( j == E || compare_edge ( edges[i], backedges[j] ) ) {
      data_ -> digraph_ . add_edge ( edges[i].first, edges[i].second );
      ++ i;
    } else if ( compare_edge ( backedges[j], edges[i] ) ) {
      ++ j;
    } else { // equality
      ++ i; ++j; // since there are no duplicates
    }
  }
}

INLINE_IF_HEADER_ONLY Digraph const DomainGraph::
digraph ( void ) const {
  return data_ -> digraph_;
}

INLINE_IF_HEADER_ONLY Annotation const DomainGraph::
annotate ( std::vector<uint64_t> const& vertices ) const {
  uint64_t D = data_ ->parameter_ . network() . size ();
  std::vector<uint64_t> limits = data_ -> parameter_ . network() . domains ();
  std::vector<std::unordered_set<uint64_t>> bounding_rect ( D );
  Domain dom ( limits );
  for ( uint64_t v : vertices ) {
    dom . setIndex ( v );
    for ( int d = 0; d < D; ++ d ) bounding_rect [ d ] . insert ( dom[d] );
  }
  std::vector<uint64_t> signature;
  for ( int d = 0; d < D; ++ d ) { 
    if ( bounding_rect [ d ] . size () > 1 ) {
      signature . push_back ( d );
    }
  }
  Annotation a;
  std::stringstream ss;
  if ( signature . size () == 0 ) {
    ss << "FP";
    bool all_on = true;
    bool all_off = true;
    for ( int d = 0; d < D; ++ d ) {
      if ( *bounding_rect[d].begin() == 0 ) all_on = false;
      else all_off = false;
    }
    if ( all_on ) ss << " ON";
    if ( all_off) ss << " OFF";
  } else if ( signature . size () == D ) {
    ss << "FC";
  } else {
    ss << "XC {";
    bool first_term = true;
    for ( uint64_t d : signature ) {
      if ( first_term ) first_term = false; else ss << ", ";
      ss << d;
    }
    ss << "}";
  }
  a . append ( ss . str () );
  return a;
}

INLINE_IF_HEADER_ONLY std::ostream& operator << ( std::ostream& stream, DomainGraph const& dg ) {
  stream << dg . digraph ();
  return stream;
}


#endif
