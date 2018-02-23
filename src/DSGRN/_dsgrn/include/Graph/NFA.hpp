/// NFA.hpp
/// Shaun Harker
/// 2018-02-20
/// MIT LICENSE

#include "NFA.h"

namespace NFA_detail {

  typedef char LabelType;

  uint64_t NodeCount = 0;

  class NFA_Node {
  public:
    NFA_Node ( void ) : index(-1) {
      ++ NodeCount;
      // std::cout << "Now there are " << NodeCount << " nodes in memory.\n";
    }

    ~NFA_Node ( void ) {
      -- NodeCount;
      // std::cout << "Now there are " << NodeCount << " nodes in memory.\n";
    }
    /// children
    std::unordered_map<NFA_Node *, std::unordered_set<LabelType>> &
    children ( void ) {
      return children_;
    }

    std::unordered_map<NFA_Node *, std::unordered_set<LabelType>> const&
    children ( void ) const {
      return children_;
    }

    /// add_edge
    void 
    add_edge ( NFA_Node * target, LabelType l ) {
      // std::cout << "adding edge " << (uint64_t) this << " -> " << (uint64_t) target << " label = " << l << "\n";
      if ( children_.count(target) == 0 ) {
        children_[target] = std::unordered_set<LabelType>();
      }
      children_[target].insert(l);
      target -> add_parent(this);
      // std::cout << "added.\n";
    }

    /// remove_edge
    void
    remove_edge ( NFA_Node * target, LabelType l ) {
      // std::cout << "removing edge " << (uint64_t) this << " -> " << (uint64_t) target << " label = " << l << "\n";
      children_[target].erase(l);
      if ( children_[target].size() == 0 ) {
        children_.erase(target);
        //target -> parents_.erase(this);
      }
      // std::cout << "removed.\n";
    }

    /// add_parent
    void
    add_parent ( NFA_Node * parent ) {
      parents_.insert(parent);
    }

    /// redirect_self_edges
    void
    redirect_self_edges ( NFA_Node * new_child ) {
      if ( num_self_edges() == 0 ) return;
      for ( auto l : children_[this]) {
        new_child -> add_edge(new_child, l);
      }
      parents_.erase(this);
      children_.erase(this);
    }

    /// redirect_in_edges
    void
    redirect_in_edges ( NFA_Node * new_child ) {
      // std::cout << "redirect_in_edges " << (uint64_t) this << " to " << (uint64_t) new_child << "\n";
      for ( auto parent : parents_ ) {
        // std::cout << " parent = " << (uint64_t) parent << "\n";
        std::unordered_set<LabelType> labels = parent -> children_[this]; //copy intentional, don't invalidate iterator below
        for ( auto label : labels ) { 
          // std::cout << "  label = " << label << "\n";
          parent -> remove_edge(this, label);
          if ( parent != new_child || label != ' ') {
            parent -> add_edge(new_child, label);
          }
        }
      }
      parents_.clear();
    }

    /// redirect_out_edges
    void
    redirect_out_edges ( NFA_Node * new_parent ) {
      // std::cout << "redirect_out_edges " << (uint64_t) this << " to " << (uint64_t) new_parent << "\n";
      for ( auto child : children_ ) {
        auto child_node = child.first;
        auto labels = child.second;
        // std::cout << " child = " << (uint64_t) child_node << "\n";
        child_node -> parents_.erase(this);
        for ( auto label : labels ) { 
          // std::cout << "  label = " << label << "\n";
          if ( new_parent != child_node || label != ' ') {
            new_parent -> add_edge(child_node, label);
          }
        }
      }
      children_.clear();
    }

    /// redirect
    void
    redirect ( NFA_Node * new_node ) {
      // std::cout << "redirect " << (uint64_t) this << " to " << (uint64_t) new_node << "\n";
      redirect_self_edges(new_node);
      redirect_in_edges(new_node);
      redirect_out_edges(new_node);
      delete this; // suicide the node.
    }

    int num_self_edges ( void ) const {
      if ( children_.count(const_cast<NFA_Node *>(this)) == 0 ) return 0;
      return children_.find(const_cast<NFA_Node *>(this))->second.size();
    }

    int num_in_edges ( void ) const {
      int result = 0;
      for ( auto x : parents_ ) {
        result += x -> children_.find(const_cast<NFA_Node *>(this))->second.size();
      }
      return result;
    }

    int num_out_edges ( void ) const {
      int result = 0;
      for ( auto x : children_ ) {
        result += x.second.size();
      }
      return result;
    }

    int num_nonself_in_edges ( void ) const {
      return num_in_edges() - num_self_edges();
    }

    int num_nonself_out_edges ( void ) const {
      return num_out_edges() - num_self_edges();
    }

    /// simplify
    void
    simplify ( void ) {
      if ( num_self_edges() != 0 ) return;
      if ( parents_.empty() || children_.empty() ) return;
      // Count in-edges
      int in_edges = 0;
      bool in_all_epsilon = true;
      for ( auto x : parents_ ) {
        auto const& C = x -> children_[this];
        in_edges += C.size();
        if ( C.size() > 1 || C.count(' ') == 0 ) {
          in_all_epsilon = false;
        }
      }
      // Count out-edges
      int out_edges = 0;
      bool out_all_epsilon = true;
      for ( auto x : children_ ) {
        auto const& C = x.second;
        out_edges += C.size();
        if ( C.size() > 1 || C.count(' ') == 0 ) {
          out_all_epsilon = false;
        }
      }
      if ( in_edges == 1 or out_edges == 1 or (in_edges == 2 and out_edges == 2) ) {
        // std::cout << "in_edges = " << in_edges << " and out_edges = " << out_edges << "\n";
        if ( in_all_epsilon ) {
          // std::cout << "simplifying " << (uint64_t) this << " via in_all_epsilon\n";
          for ( auto x : parents_ ) {
            for ( auto y : children_ ) {
              for ( auto l : y.second ) {
                x -> add_edge(y.first,l);
              }
            }
          }
          for ( auto x : parents_ ) {
            x -> remove_edge(this, ' ');
          }
          for ( auto y : children_ ) {
            y.first -> parents_.erase(this);
          }
          delete this;
        } else if ( out_all_epsilon ) {
          // std::cout << "simplifying " << (uint64_t) this << " via out_all_epsilon\n";
          for ( auto y : children_ ) {
            for ( auto x : parents_ ) {
              for ( auto l : x -> children_[this] ) {
                x -> add_edge(y.first,l);
              }
            }
          }
          for ( auto x : parents_ ) {
            std::unordered_set<LabelType> labels = x -> children_[this];
            for ( auto l : labels ) {
              x -> remove_edge(this, l);
            }
          }
          for ( auto y : children_ ) {
            y.first -> parents_.erase(this);
          }
          delete this;
        }
      }

    }
    /// data
    std::unordered_map<NFA_Node *, std::unordered_set<LabelType>> children_;
    std::unordered_set<NFA_Node *> parents_;
    uint64_t index;
  };

  bool have_same_self_edges ( NFA_Node * lhs, NFA_Node * rhs ) {
    if ( lhs->num_self_edges() != rhs->num_self_edges() ) {
      return false;
    } else if ( lhs->num_self_edges() == 0 ) {
      return true;
    } else {
      for ( auto l : lhs->children()[lhs]) {
        if ( rhs->children()[rhs].count(l) == 0 ) {
          return false;
        }
      }
      for ( auto l : rhs->children()[rhs]) {
        if ( lhs->children()[lhs].count(l) == 0 ) {
          return false;
        }
      }
    }
    return true;
  }

  bool have_disjoint_self_edges ( NFA_Node * lhs, NFA_Node * rhs ) {
    if ( lhs->children().count(lhs) == 0 || rhs->children().count(rhs) == 0) {
      return true;
    }
    for ( auto l : lhs->children()[lhs]) {
      if ( rhs->children()[rhs].count(l) > 0 ) return false;
    }
    for ( auto l : rhs->children()[rhs]) {
      if ( lhs->children()[lhs].count(l) > 0 ) return false;
    }
    return true;
  }

  bool contains_self_edges ( NFA_Node * lhs, NFA_Node * rhs ) {
    if ( rhs->children().count(rhs) == 0 ) {
      return true;
    }
    if ( lhs->children().count(lhs) == 0 ) {
      return false;
    }
    for ( auto l : rhs->children()[rhs]) {
      if ( lhs->children()[lhs].count(l) == 0 ) return false;
    }
    return true;
  }

}

inline
NFA::NFA ( void ) {}

inline
NFA::NFA ( char c ) {
  using namespace NFA_detail;
  initial_ = new NFA_Node();
  final_ = new NFA_Node();
  initial_->add_edge(final_, c);
}

inline
NFA::NFA ( NFA_detail::NFA_Node * I, NFA_detail::NFA_Node * F ) : initial_(I), final_(F) {}

inline
NFA_detail::NFA_Node *
NFA::initial ( void ) const {
  return const_cast<NFA_detail::NFA_Node *>(initial_);
}

inline
NFA_detail::NFA_Node *
NFA::final ( void ) const {
  return const_cast<NFA_detail::NFA_Node *>(final_);
}

inline NFA
NFA::alternation ( NFA && lhs, NFA && rhs) {
  using namespace NFA_detail;
  // std::cout << "alternation\n";
  NFA_Node * I;
  NFA_Node * F;
  bool merged_initial;
  auto LI = lhs.initial();
  auto LF = lhs.final();
  auto RI = rhs.initial();
  auto RF = rhs.final();
  // L is a single node connected to itself
  // and adding an edge across R suffices because L-self-edges can be all found at either RI or RF (but not both, and no extras)
  if ( LI == LF && LI -> num_nonself_in_edges() == 0 ) {
    if ( (have_same_self_edges (LI, RI) && RF -> num_self_edges() == 0) || 
         (have_same_self_edges (LF, RF) && RI -> num_self_edges() == 0) ) {
      RI->add_edge(RF, ' ');
      lhs.delete_nodes();
      return NFA(RI, RF);   
    }
  }
  // R is a single node connected to itself
  // and adding an edge across L suffices because R-self-edges can be all found at either LI or LF (but not both, and no extras)
  if ( RI == RF && RI -> num_nonself_in_edges() == 0 ) {
    if ( (have_same_self_edges (LI, RI) && LF -> num_self_edges() == 0) || 
         (have_same_self_edges (LF, RF) && LI -> num_self_edges() == 0) ) {
      LI->add_edge(LF, ' ');
      rhs.delete_nodes();
      return NFA(LI, LF);   
    }
  }

  if ( LI == LF ) {
    // If RI has no in-edges then we can add an epsilon transition from RI to LI=LF instead of creating a new node
    if ( RI -> num_in_edges() == 0 ) {
      RI->add_edge(LI, ' ');
      I = RI;
    } else {
      I = new NFA_Node();
      I->add_edge(LI, ' ');
      I->add_edge(RI, ' ');
    }
    // If RF has no out-edges then we can add an epsilon transition from LI=LF to RF instead of creating a new node
    if ( RF -> num_out_edges() == 0 ) {
      LF->add_edge(RF, ' ');
      F = RF;
    } else {
      F = new NFA_Node();
      LF->add_edge(F, ' ');
      RF->add_edge(F, ' ');
    }
    return NFA(I, F);
  }
  if ( RI == RF ) {
    // If LI has no in-edges then we can add an epsilon transition from LI to RI=RF instead of creating a new node
    if ( LI -> num_in_edges() == 0 ) {
      LI->add_edge(RI, ' ');
      I = LI;
    } else {
      I = new NFA_Node();
      I->add_edge(LI, ' ');
      I->add_edge(RI, ' ');
    }
    // If LF has no out-edges then we can add an epsilon transition from RI=RF to LF instead of creating a new node
    if ( LF -> num_out_edges() == 0 ) {
      RF->add_edge(LF, ' ');
      F = LF;
    } else {
      F = new NFA_Node();
      LF->add_edge(F, ' ');
      RF->add_edge(F, ' ');
    }
    return NFA(I, F);
  }

  // TODO: are there any other LI==LF or RI==RF improvements?

  if ( LI == LF || RI == RF ) throw std::logic_error("NFA::alternation. Unexpected case.\n");

  if (have_same_self_edges (LI, RI) && 
      LI->num_nonself_in_edges() == 0 &&
      RI->num_nonself_in_edges() == 0 ) {
    // std::cout << " merging initial\n";
    I = LI;
    RI -> redirect(I);
  } else {
    I = new NFA_Node();
    I->add_edge(LI, ' ');
    I->add_edge(RI, ' ');
  }
  if (have_same_self_edges (LF, RF) &&
      LF->num_nonself_out_edges() == 0 && 
      RF->num_nonself_out_edges() == 0 ) {
    // std::cout << " merging final\n";
    F = RF;
    LF -> redirect(F);
    return NFA(I, F);
  } else {
    F = new NFA_Node();
    LF -> add_edge(F, ' ');
    RF -> add_edge(F, ' ');
    return NFA(I, F);
  }
}

inline NFA
NFA::concatenation ( NFA && lhs, NFA && rhs) {
  using namespace NFA_detail;
  // std::cout << "concatenation\n";
  if (lhs.final()->num_out_edges() == 0 || 
      rhs.initial()->num_in_edges() == 0 || 
      (lhs.final()->num_nonself_out_edges() == 0 && rhs.initial()->num_nonself_in_edges() == 0 && have_same_self_edges (lhs.final(), rhs.initial() ) ) ) {
    auto I = lhs.initial();
    auto F = rhs.final();
    lhs.final() -> redirect(rhs.initial());
    if (lhs.initial() == lhs.final() ) I = rhs.initial();
    return NFA(I, F);
  } else {
    auto I = lhs.initial();
    auto F = rhs.final();
    lhs.final() -> add_edge(rhs.initial(), ' ');
    return NFA(I, F);
  }
}

inline NFA
NFA::kleene ( NFA && nfa ) {
  using namespace NFA_detail;
  // std::cout << "kleene\n";
  if ( nfa.initial() == nfa.final() ) {
    return NFA(nfa.initial(), nfa.final());
  }
  if ( nfa.initial()->num_in_edges() == 0 && nfa.final()->num_out_edges() == 0 ) {
    nfa.final() -> redirect(nfa.initial());
    return NFA(nfa.initial(), nfa.initial());
  }
  return question(plus(std::move(nfa)));
}

inline NFA
NFA::plus ( NFA && nfa ) {
  // one or more times
  using namespace NFA_detail;
  // std::cout << "plus\n";
  if ( nfa.initial() == nfa.final() ) {
    return NFA(nfa.initial(), nfa.final());
  }
  nfa.final()->add_edge(nfa.initial(), ' ');
  return NFA(nfa.initial(), nfa.final());
}

inline NFA
NFA::question ( NFA && nfa ) {
  // note: can't simplify by making new final same as old final because then mulitiple paths would match same string
  using namespace NFA_detail;
  // std::cout << "question\n";
  if ( nfa.initial() == nfa.final() ) {
    return NFA(nfa.initial(), nfa.final());
  }
  if ( nfa.initial()->num_in_edges() == 0 && nfa.final()->num_out_edges() == 0 ) {
    nfa.initial()->add_edge(nfa.final(),' ');
    return NFA(nfa.initial(), nfa.final());
  }
  if ( nfa.initial()->num_in_edges() == 0 ) {
    auto F = new NFA_Node();
    nfa.initial()->add_edge(F, ' ');
    nfa.final()->add_edge(F, ' ');
    return NFA(nfa.initial(), F);
  }
  if ( nfa.final()->num_out_edges() == 0 ) {
    auto I = new NFA_Node();
    I->add_edge(nfa.initial(), ' ');
    I->add_edge(nfa.final(), ' ');
    return NFA(I, nfa.final());
  }  
  auto I = new NFA_Node();
  auto F = new NFA_Node();
  I->add_edge(nfa.initial(), ' ');
  I->add_edge(F, ' ');
  nfa.final()->add_edge(F, ' ');
  return NFA(I, F);
}

inline std::unordered_set<NFA_detail::NFA_Node *>
NFA::nodes ( void ) {
  using namespace NFA_detail;
  std::unordered_set<NFA_Node *> explored;
  std::stack<NFA_Node *> nodes_;
  nodes_.push(initial());
  while ( not nodes_ . empty() ) {
    auto x = nodes_.top();
    nodes_.pop();
    if ( explored.count(x) ) continue;
    explored.insert(x);
    for ( auto y : x -> children() ) {
      nodes_.push(y.first);
    }
  }
  return explored;
}

inline void
NFA::delete_nodes ( void ) {
  for ( auto n : nodes() ) delete n;
  return; 
}

inline void 
NFA::contract ( void ) {
  for ( auto n : nodes() ) {
    if ( n != initial() && n != final() ) {
      n -> simplify();
    }
  }
  return;
}

inline std::string 
NFA::graphviz ( void ) const {
  using namespace NFA_detail;
  std::stringstream ss;
  ss << "digraph G {\n";
  for ( uint64_t i = 0; i < num_vertices(); ++ i ) {
    ss << i << " [label=\"" << i 
       << ((i == initial_idx_) ? "(initial)" : "") << ((i == final_idx_) ? "(final)" : "") << "\"];\n";
  }

  // Queue the remaining children.
  for ( uint64_t i = 0; i < num_vertices(); ++ i ) {
    for ( auto label_set : adjacencies(i) ) {
      auto label = label_set.first;
      auto set = label_set.second;
      for ( auto j : set ) ss << i << " -> " << j << "[label=\"" << label << "\"]\n";
    }
  }
  ss << "}\n";
  return ss.str();
}

inline 
NFA::NFA (std::string const& regex) {
  /// Parse regex between i and j
  typedef std::pair<int,int> Term;
  std::function<NFA(Term)> parse = [&](Term t) {
    // Algorithm. Precedence is parentheses, kleene star, then concatenation, then alternation.
    // Step 0. If single character result basic result.
    // Step 1. Find all top level terms. (parentheses pairs or single characters) Recurse.
    // Step 2. For each pair, check if the term is kleene starred. 
    // Step 3. Concatenate consecutive terms between | bars
    // Step 4. Combine concatenations with alternation
    int i = t.first;
    int j = t.second;
    if ( j == i + 1 ) return NFA(regex[i]); // Single character
    std::vector<std::vector<NFA>> expression; // alternations of concatenations
    std::vector<NFA> concatenation_string;
    while ( i < j ) {
      if (regex[i] == '*' || regex[i] == '+' || regex[i] == '?') {
        if ( concatenation_string.size() == 0 ) throw std::invalid_argument("NFA constructor: invalid regex " + regex);
        NFA back = concatenation_string.back();
        concatenation_string.pop_back();
        switch ( regex[i] ) {
          case '*': back = NFA::kleene(std::move(back)); break;
          case '+': back = NFA::plus(std::move(back)); break;
          case '?': back = NFA::question(std::move(back)); break;
        }
        concatenation_string.push_back(back);
        ++ i;
        continue;
      }
      if (regex[i] == '(') {
        int openminusclose = 1;
        int k = i+1;
        for (; k < j; ++ k ) {
          if ( regex[k] == '(') ++ openminusclose;
          if ( regex[k] == ')') -- openminusclose;
          if ( openminusclose == 0 ) break;
        }
        if ( openminusclose != 0 ) {
          throw std::invalid_argument("NFA constructor: invalid regex " + regex );
        }
        concatenation_string.push_back(parse({i+1, k})); // [i+1, k) is inside of parentheses. 
        i = k+1;
        continue;
      } 
      if (regex[i] == '|') {
        expression.push_back(concatenation_string);
        concatenation_string.clear();
        ++ i;
        continue;
      }
      concatenation_string.push_back(parse({i, i+1})); // [i, i+1) is a character
      ++ i;
    }
    expression.push_back(concatenation_string);
    // perform concatenations:
    std::vector<NFA> alternation_string;
    for ( auto cs : expression ) {
      if ( cs.size() == 0 ) continue;
      NFA nfa = cs[0];
      for ( int i = 1; i < cs.size(); ++ i ) {
        nfa = NFA::concatenation(std::move(nfa), std::move(cs[i]));
      }
      alternation_string.push_back(nfa);
    }
    // perform alternations
    if ( alternation_string.size() == 0 ) throw std::invalid_argument("NFA constructor: invalid regex " + regex);
    NFA result = alternation_string[0];
    for ( int i = 1; i < alternation_string.size(); ++ i ) {
      result = NFA::alternation(std::move(result), std::move(alternation_string[i]));
    }
    return result;
  };
  auto result = parse({0, regex.size()});
  result.contract();
  initial_ = result.initial_;
  final_ = result.final_;
  finalize();
}

inline void 
NFA::finalize ( void ) {
  using namespace NFA_detail;
  std::unordered_map<NFA_Node *, uint64_t> node_index;
  for ( auto n : nodes() ) node_index[n] = add_vertex();
  std::unordered_set<uint64_t> explored;
  std::stack<NFA_Node *> nodes_;
  nodes_.push(initial());
  // std::cout << "starting finalize while loop.\n";
  while ( not nodes_ . empty() ) {
    auto x = nodes_.top();
    nodes_.pop();
    auto i = node_index[x];
    if ( explored.count(i) ) continue;
    explored.insert(i);
    // std::cout << "preorder vertex " << i << "\n";
    for ( auto y : x -> children() ) {
      nodes_.push(y.first);
      auto j = node_index[y.first];
      // std::cout << " child " << j << "\n";
      auto labels = y.second;
      for ( auto label : labels ) { 
        // std::cout << "add edge " << i << " " << j << " " << label << "\n";
        add_edge(i,j,label);
        // std::cout << "added.\n";
      }
    }
  }
  // std::cout << "finalize while loop complete.\n";
  initial_idx_ = node_index[initial()];
  final_idx_ = node_index[final()];  
  for ( auto n : nodes () ) delete n;
  initial_ = nullptr;
  final_ = nullptr;
}

