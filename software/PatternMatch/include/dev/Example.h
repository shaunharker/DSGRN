/// Example.h
/// Shaun Harker
/// (see notes below for more details)
#ifndef EXAMPLEPATTERNMATCH_H
#define EXAMPLEPATTERNMATCH_H

/// patternMatch  
///  INPUT:   A labelled directed graph g with methods "size" "adjacencies" and "label"
///           A pattern p which is an array of labels.           
///  OUTPUT:  For mode == 0, the function returns 1 if there is a "match" and 0 otherwise
///           For mode == 1, the function returns the number of matches
template<class LabelledGraph>
uint64_t 
patternMatch ( LabelledGraph const& g, 
               Pattern const& p, 
               int mode = 0) {
  typedef std::pair<uint64_t, uint64_t> State; // (wall_index, pattern_index) pair
  typedef std::vector<uint8_t> Label;
  auto is_nonconsuming_match = [&]( Label const& x, Label const& y ) {
    uint64_t n = x . size (); // == y . size ();
    for ( int64_t i = 0; i < n; ++ i ) {
      if ( x[i] & y[i] & 3 == 0 ) return false;
    }
    return true;
  };
  auto is_consuming_match = [&]( Label const& x, Label const& y ) {
    uint64_t n = x . size (); // == y . size ();
    uint64_t consume_count = 0;
    for ( int64_t i = 0; i < n; ++ i ) {
      char c = x[i] & y[i];
      if ( x[i] & y[i] == 0 ) return false;
      if ( c & 12 ) ++ consume_count;
    }
    return (consume_count == 1);
  };
  std::unordered_map<State, uint64_t> memoization_table;
  auto match_count = [&] ( uint64_t initial_wall ) {
    std::stack<State> dfs_stack;
    std::stack<uint64_t> result_stack;
    dfs_stack . push_back ( { initial_wall, 0 } );
    result_stack . push_back ( 0 );
    // Recursion loop
    while ( not dfs_stack . empty () ) {
      State state = dfs_stack . top ();
      dfs_stack . pop ();
      if ( memoization_table . count ( state ) == 0 ) {
        // FIRST VISIT to "state" (Preorder step)
        memoization_table [ state ] = -1;
        dfs_stack . push ( state );
        result_stack . push ( 0 ); // push to result_stack on preorder step
        uint64_t wi = state . first;
        uint64_t pi = state . second;
        // Not a match, so recurse to children states
        Label pattern_label = pattern[pi];
        Label wall_label = g . label (wi);
        if ( is_nonconsuming_match ( pattern_label, wall_label ) ) {
          // Recurse without consuming
          for ( uint64_t next_wi : g . adjacencies ( wi ) ) {
            dfs_stack . push ( { next_wi, pi } );
          }
        }
        if ( is_consuming_match ( pattern_label, wall_label ) ) {
          if ( pi + 1 == p . size () ) { 
            // Found a match
            result_stack . top () = 1;
            if ( mode == 0 ) return 1; // In mode 0, we are done!
          } else {
            // Recurse with pattern element consumed
            for ( uint64_t next_wi : g . adjacencies ( wi ) ) {
              dfs_stack . push ( { next_wi, pi + 1 } );
            } 
          }
        } 
      } else { 
        // SECOND VISIT OR LATER to "state"
        if ( memoization_table [ state ] == -1 ) {
          // SECOND VISIT (Postorder step)
          memoization_table [ state ] = result_stack . top ();
          result_stack . pop (); // pop from result_stack on postorder step
        }
        result_stack . top () += memoization_table [ state ];
      }
    }
    return result_stack.top();
  };
  // Loop through all initial walls and sum match counts
  uint64_t result = 0;
  for ( uint64_t wi = 0; wi < g . size(); ++ wi ) {
    result += match_count ( wi );
  }
  return result;
}


/*
  NOTES:

Wall labels are lists of subsets of {I, D, M, m}, one for each variable.
Each such subset we call a variable label.

We can nicely represent any such variable label with an integer 0 <= i < 16
such that the bit representation is 
M m I D
0 0 0 0  {}    0
0 0 0 1  {D}   1
0 0 1 0  {I}   2
0 0 1 1  {I,D} 3
0 1 0 0  {m}   4
... etc ...
This representation is nice since we can use bitwise techniques.

Pattern labels are the same as wall labels, and they obey the property that their 
constituent variable labels can only be 1, 2, 5, and 10 (i.e. {D}, {I}, {m, D}, {M, I}).

A variable label P is said to be a match to a variable label W iff
P & W != 0

A variable label P is said to be a "nonconsuming" (a.k.a. "non-essential", "intermediate") match to a variable label W iff
P & W & 3 != 0   (In other words, P had either an I or a D and W had that letter as well)

A variable label P is said to be a "consumable" (a.k.a. "essential", "extremal") match to a variable label W iff
P & W & 12 != 0  (In other words, P had either an m or an M and W had that letter as well)

Note that a match is either consumable, nonconsumable, or both.

Here & denotes the "bitwise-AND" as implemented in C/C++

A wall label (or pattern label) is simply a length n list of variable labels.
We say a pattern label is a consumable match to a wall label if each corresponding pair
of variable labels are a match and precisely one of these matches is consumable.

(Note that if more than one were consumable this would indicate a bug in our program. BREE CHECK THIS)

We say a pattern label is a non-consumable match to a wall label if each corresponding pair
of variable labels are a non-consumable match.
*/

/// patternMatch
///
///  INPUT: A directed graph G equipped with a method
///             "adjacencies" and a method "label"
///         A pattern P, which is an array of "Label" objects as described above
///
///  BACKGROUND:
///    Given the labelled graph G and the pattern P, we create (conceptually) a new graph
///    G' with vertices { (v, i) : v \in G and 0 <= i < length(P) } \cup {*}
///       and edges  (u,i) -> (v,i)   if u->v in G and label(u) is a non-consumable match to P(i)
///                  (u,i) -> (v,i+1) if u->v in G and label(u) is a consumable match to P(i)
///                  (u,|P|-1) -> *   if u->v in G and label(u) is a consumable match to P(|P|-1)
///
///    Assumption: G' so described is acyclic. Equivalently, there does not exist
///                a cycle in G and a pattern label p such that each wall label in the 
///                cycle is a non-consumable match with the pattern label p.
///
///    Denote #((v,i) --> *) to be the number of distinct paths from (v,i) to * in G'.
///
///  OUTPUT:
///    For mode == 0, the function returns 1 if there exists a path from (u,0) to * for some u \in G.
///    For mode == 1, the function return \sum_{v \in G} #((v,0) --> *)
///
///  ALGORITHM:
///    The acyclic property of G' allows us to perform the computation using a simple recursion.
///    In its basic form this algorithm is not difficult
///      #(v --> *) = \sum_{u \in Adj(v)} #(u --> *),   where #(x --> y) denotes the number of 
///                                                           paths from x to y
///      #(* --> *) = 1
///    We use "memoization" to avoid recomputing, which results in a polynomial time algorithm.
///    In a C++ implementation we need to concern ourselves with the stack overflowing for large
///    graphs so unfortunately the recursion should be implemented with a dynamically allocated stack
///    rather than recursive function calls. This complicates matters, especially along with 
///    memoization. Our technique for handling this is as follow:
///       We loop through all starting walls (u, 0) in G'. 
///       We use a stack "dfs_stack" to perform a depth-first-search traversal of G' from our starting vertex
///       We use a stack "results_stack" to accumulate sums of paths to be passed back to the caller
///       (this sidesteps the need to know who the caller was, surprisingly)
///       This traversal has three types of "visits":
///       First visit: (preorder)
///         The first call to ask for the number of paths to * from a given state
///         A 0 is placed on a result stack, and children states will add their contribution into this number later
///         The state is pushed immediately back on the stack, but won't be popped again until all descendants are
///         processed (due to the nature of stacks)
///         We place the special value -1 in the memoization table so we will know on the next visit that it is the
///         second visit.
///       Second visit: (postorder)
///         All the children of the state have been processed and indeed postordered. 
///         The top of the results stack is the number of paths to * from the current state. 
///         We pop this number in the memoization table, replacing the -1.
///       All later visits:
///         We access the memoization table result and add it to the top of the results stack, which will
///         correspond to such immediate predecessor in the directed graph.
///       When the DFS is over, we will have the number of paths as the sole remaining item on the results stack.
///       We add these up over our loop over starting walls.

#endif
