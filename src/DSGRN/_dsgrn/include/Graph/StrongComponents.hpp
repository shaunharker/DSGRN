/// StrongComponents.hpp
/// Shaun Harker
/// 2015-05-24

#pragma once

#ifndef INLINE_IF_HEADER_ONLY
#define INLINE_IF_HEADER_ONLY
#endif

#include "StrongComponents.h"

INLINE_IF_HEADER_ONLY Components
StrongComponents ( Digraph const digraph ) {
  int64_t N = (int64_t) digraph . size ();
  std::vector<uint64_t> output_vertices;
  std::vector<bool> output_scc_root;
  output_vertices . reserve ( N );
  output_scc_root . reserve ( N );
  std::vector<bool> output_recurrent;
  std::vector<bool> explored ( N, false );
  std::vector<bool> committed ( N, false );
  std::vector<bool> duplicates ( N, false );
  std::vector<bool> self_connected (N, false);
  std::vector<int64_t> preorder ( N, 0 );
  std::deque<int64_t> LOWLINK, DFS, cleanDFS;
  std::deque<uint64_t> S;
  int64_t n = 0;
  LOWLINK . push_back ( -1 );
  for ( int64_t v = 0; v < N; ++ v ) {
    if ( not explored [ v ] ) {
      DFS . push_back ( v+1 );
      while ( not DFS . empty () ) {
        int64_t u = DFS . back ();
        DFS . pop_back ();
        if ( u > 0 ) {
          // PREORDER
          u = u - 1;
          //std::cout << "Preorder(" << u << ")\n";
          if ( not explored [ u ] ) {
            DFS . push_back ( -u-1);
            explored [ u ] = true;
            preorder [ u ] = n;
            int64_t low = n;
            ++ n;
            std::vector<uint64_t> const& W = digraph . adjacencies ( u );
            for ( int64_t w : W ) {
              if ( u == w ) self_connected [ u ] = true;
              if ( explored [ w ] ) {
                if ( not committed [ w ] ) {
                  low = std::min(low, preorder[w] );
                }
              } else {
                DFS . push_back ( w + 1 );
                if ( (int64_t) DFS . size () > 2L*N ) { 
                  duplicates . assign ( N, false );
                  while ( not DFS . empty () ) {
                    int64_t x = DFS . back ();
                    DFS . pop_back ();
                    int64_t y = std::abs(x) - 1;
                    if ( duplicates [ y ] ) continue;
                    duplicates [ y ] = true;
                    cleanDFS . push_front ( x );
                  }
                  std::swap ( DFS, cleanDFS );
                }
              }
            }
            LOWLINK . push_back ( low );
            S . push_back ( (uint64_t) u );
          }
        } else {
          // POSTORDER
          u = -u - 1;
          //std::cout << "Postorder(" << u << ")\n";
          int64_t lowlink = LOWLINK . back ();
          LOWLINK . pop_back ();

          if ( lowlink == preorder [ u ] ) {
            // Record if component is recurrent or not.
            if ( S . back () == u &&
              not self_connected [ u ] ) { 
              output_recurrent . push_back ( false );
            } else { 
              output_recurrent . push_back ( true );
            }
            // Record the component
            do {
              int64_t w = S . back ();
              S . pop_back ();
              output_vertices . push_back ( (uint64_t) w );
              output_scc_root . push_back ( false );
              committed [ w ] = true;
            } while ( not committed [ u ] );
            output_scc_root . back () = true;
          }
          int64_t & low = LOWLINK . back ();
          low = std::min ( lowlink, low );
        }
      }
    }
  }
  std::reverse ( output_vertices . begin (), output_vertices . end () );
  std::reverse ( output_scc_root . begin (), output_scc_root . end () );
  std::reverse ( output_recurrent . begin (), output_recurrent . end () );
  return Components ( output_vertices, 
                      output_scc_root, 
                      output_recurrent );
}
