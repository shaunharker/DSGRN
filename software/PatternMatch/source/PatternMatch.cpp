/// PatternMatch.cpp
/// Shaun Harker and Bree Cummins
/// 2016-03-19

#include "PatternMatch.h"

bool
QueryCycleMatch ( MatchingGraph const& mg ) {
  uint64_t N = mg . SearchGraph() . size ();
  for ( uint64_t domain = 0; domain < N; ++ domain ) {
    uint64_t start = mg . vertex ( domain, mg . PatternGraph . root () );
    uint64_t end = mg . vertex ( domain, mg . PatternGraph . leaf () );
    // TODO MAKE SURE start/end exist
    std::unordered_set<uint64_t> explored; 
    std::stack<uint64_t> dfs_stack;
    dfs_stack . push ( start );
    while ( not dfs_stack . empty () ) {
      uint64_t vertex = dfs_stack . top ();
      dfs_stack . pop ();
      if ( vertex == end ) return true;
      explored . insert ( vertex );
      for ( uint64_t next_vertex : mg . adjacencies ( vertex ) ) {
        if ( explored [ vertex ] . count () == 0 ) { 
          dfs_stack . push ( next_vertex );
        }
      }
    }
  }
  return false;
}

bool
QueryPathMatch ( MatchingGraph const& mg ) {
  uint64_t N = mg . SearchGraph() . size ();
  for ( uint64_t domain = 0; domain < N; ++ domain ) {
    uint64_t start = mg . vertex ( domain, mg . PatternGraph() . root () );
    uint64_t end = mg . PatternGraph() . leaf ();
    // TODO MAKE SURE start/end exist
    std::unordered_set<uint64_t> explored; 
    std::stack<uint64_t> dfs_stack;
    dfs_stack . push ( start );
    while ( not dfs_stack . empty () ) {
      uint64_t vertex = dfs_stack . top ();
      dfs_stack . pop ();
      if ( mg.position(vertex) == end ) return true;
      explored . insert ( vertex );
      for ( uint64_t next_vertex : mg . adjacencies ( vertex ) ) {
        if ( explored [ vertex ] . count () == 0 ) { 
          dfs_stack . push ( next_vertex );
        }
      }
    }
  }
  return false;
}


uint64_t
CountCycleMatches ( MatchingGraph const& mg ) {
  uint64_t N = mg . SearchGraph() . size ();
  uint64_t result = 0;
  // Loop through starting domains
  for ( uint64_t domain = 0; domain < N; ++ domain ) {
    // Count the number of paths from (start_domain, root) to (start_domain, leaf)
    uint64_t start = mg . vertex ( domain, mg . PatternGraph . root () );
    uint64_t end = mg . vertex ( domain, mg . PatternGraph . leaf () );
    // TODO MAKE SURE start/end exist
    // We use a data structure which will store the number of paths from a vertex to "end"
    std::unordered_map<uint64_t, uint64_t> memoization_table; 
    memoization_table[end] = 1;
    // The algorithm takes the form of a depth-first-search traversal
    std::stack<uint64_t> dfs_stack;
    dfs_stack . push ( start );
    while ( not dfs_stack . empty () ) {
      uint64_t vertex = dfs_stack . top ();
      dfs_stack . pop ();
      if ( memoization_table . count ( vertex ) == 0 ) {
        // FIRST VISIT to "vertex" (Preorder step)
        memoization_table [ vertex ] = -1;
        // Push the vertex onto dfs_stack again to arrange for second visit
        dfs_stack . push ( vertex );
        result_stack . push ( 0 ); // push to result_stack on preorder step
        for ( uint64_t next_vertex : mg . adjacencies ( vertex ) ) {
          dfs_stack . push ( next_vertex );
        }
      } else { 
        // SECOND VISIT OR LATER to "vertex"
        if ( memoization_table [ vertex ] == -1 ) {
          // SECOND VISIT (Postorder step)
          memoization_table [ vertex ] = result_stack . top ();
          result_stack . pop (); // pop from result_stack on postorder step
        }
        result_stack . top () += memoization_table [ vertex ];
      }
    }
    result += memoization_table[start];
  }
  return result;
}

uint64_t
CountPathMatches ( MatchingGraph const& mg ) {
  uint64_t N = mg . SearchGraph() . size ();
  // Loop through starting domains
  for ( uint64_t domain = 0; domain < N; ++ domain ) {
    // Count the number of paths from (start_domain, root) to (start_domain, leaf)
    uint64_t start = mg . vertex ( domain, mg . PatternGraph . root () );
    uint64_t end = mg . PatternGraph() . leaf ();
    // TODO MAKE SURE start/end exist
    // We use a data structure which will store the number of paths from a vertex to "end"
    std::unordered_map<uint64_t, uint64_t> memoization_table; 
    memoization_table[end] = 1;
    // The algorithm takes the form of a depth-first-search traversal
    std::stack<uint64_t> dfs_stack;
    dfs_stack . push ( start );
    while ( not dfs_stack . empty () ) {
      uint64_t vertex = dfs_stack . top ();
      dfs_stack . pop ();
      if ( memoization_table . count ( vertex ) == 0 ) {
        // FIRST VISIT to "vertex" (Preorder step)
        memoization_table [ vertex ] = -1;
        // Push the vertex onto dfs_stack again to arrange for second visit
        dfs_stack . push ( vertex );
        result_stack . push ( 0 ); // push to result_stack on preorder step
        for ( uint64_t next_vertex : mg . adjacencies ( vertex ) ) {
          dfs_stack . push ( next_vertex );
        }
      } else { 
        // SECOND VISIT OR LATER to "vertex"
        if ( memoization_table [ vertex ] == -1 ) {
          // SECOND VISIT (Postorder step)
          memoization_table [ vertex ] = result_stack . top ();
          result_stack . pop (); // pop from result_stack on postorder step
        }
        result_stack . top () += memoization_table [ vertex ];
      }
    }
  }
}
