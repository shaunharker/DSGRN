/// ThompsonsConstruction.hpp
/// Shaun Harker
/// 2018-02-20
/// MIT LICENSE

#include "ThompsonsConstruction.h"



namespace ThompsonsConstruction_detail {
  typedef char LabelType;
  class Node;
  uint64_t NodeCount = 0;

  struct ThompsonsConstruction {

    /// ThompsonsConstruction
    ThompsonsConstruction ( void );

    /// ThompsonsConstruction
    ThompsonsConstruction (char a);
    
    /// ThompsonsConstruction
    ThompsonsConstruction ( Node * initial, Node * final );

    /// finalize
    void 
    finalize ( void );
    
    /// initial
    Node *
    initial ( void ) const;

    /// final
    Node *
    final ( void ) const;

    /// nodes
    std::unordered_set<Node *>
    nodes ( void );

    /// delete_nodes
    void
    delete_nodes ( void );

    /// contract
    ///   Contract nodes together to give an equivalent but simpler ThompsonsConstruction
    void
    contract ( void );

    Node * initial_;
    Node * final_;
  };

  /// alternation
  ThompsonsConstruction
  alternation ( ThompsonsConstruction && lhs, ThompsonsConstruction && rhs);

  /// concatenation
  ThompsonsConstruction
  concatenation ( ThompsonsConstruction && lhs, ThompsonsConstruction && rhs);

  /// kleene
  ThompsonsConstruction
  kleene ( ThompsonsConstruction && nfa );

  /// plus
  ThompsonsConstruction
  plus ( ThompsonsConstruction && nfa );

  /// question
  ThompsonsConstruction
  question ( ThompsonsConstruction && nfa );

  class Node {
  public:
    Node ( void ) : index(-1) {
      ++ NodeCount;
      // std::cout << "Now there are " << NodeCount << " nodes in memory.\n";
    }

    ~Node ( void ) {
      -- NodeCount;
      // std::cout << "Now there are " << NodeCount << " nodes in memory.\n";
    }
    /// children
    std::unordered_map<Node *, std::unordered_set<LabelType>> &
    children ( void ) {
      return children_;
    }

    std::unordered_map<Node *, std::unordered_set<LabelType>> const&
    children ( void ) const {
      return children_;
    }

    /// add_edge
    void 
    add_edge ( Node * target, LabelType l ) {
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
    remove_edge ( Node * target, LabelType l ) {
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
    add_parent ( Node * parent ) {
      parents_.insert(parent);
    }

    /// redirect_self_edges
    void
    redirect_self_edges ( Node * new_child ) {
      if ( num_self_edges() == 0 ) return;
      for ( auto l : children_[this]) {
        new_child -> add_edge(new_child, l);
      }
      parents_.erase(this);
      children_.erase(this);
    }

    /// redirect_in_edges
    void
    redirect_in_edges ( Node * new_child ) {
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
    redirect_out_edges ( Node * new_parent ) {
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
    redirect ( Node * new_node ) {
      // std::cout << "redirect " << (uint64_t) this << " to " << (uint64_t) new_node << "\n";
      redirect_self_edges(new_node);
      redirect_in_edges(new_node);
      redirect_out_edges(new_node);
      delete this; // suicide the node.
    }

    int num_self_edges ( void ) const {
      if ( children_.count(const_cast<Node *>(this)) == 0 ) return 0;
      return children_.find(const_cast<Node *>(this))->second.size();
    }

    int num_in_edges ( void ) const {
      int result = 0;
      for ( auto x : parents_ ) {
        result += x -> children_.find(const_cast<Node *>(this))->second.size();
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
    std::unordered_map<Node *, std::unordered_set<LabelType>> children_;
    std::unordered_set<Node *> parents_;
    uint64_t index;
  };

  bool have_same_self_edges ( Node * lhs, Node * rhs ) {
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

  bool have_disjoint_self_edges ( Node * lhs, Node * rhs ) {
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

  bool contains_self_edges ( Node * lhs, Node * rhs ) {
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

  inline ThompsonsConstruction
  alternation ( ThompsonsConstruction && lhs, ThompsonsConstruction && rhs) {
    // std::cout << "alternation\n";
    Node * I;
    Node * F;
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
        return ThompsonsConstruction(RI, RF);   
      }
    }
    // R is a single node connected to itself
    // and adding an edge across L suffices because R-self-edges can be all found at either LI or LF (but not both, and no extras)
    if ( RI == RF && RI -> num_nonself_in_edges() == 0 ) {
      if ( (have_same_self_edges (LI, RI) && LF -> num_self_edges() == 0) || 
           (have_same_self_edges (LF, RF) && LI -> num_self_edges() == 0) ) {
        LI->add_edge(LF, ' ');
        rhs.delete_nodes();
        return ThompsonsConstruction(LI, LF);   
      }
    }

    if ( LI == LF ) {
      // If RI has no in-edges then we can add an epsilon transition from RI to LI=LF instead of creating a new node
      if ( RI -> num_in_edges() == 0 ) {
        RI->add_edge(LI, ' ');
        I = RI;
      } else {
        I = new Node();
        I->add_edge(LI, ' ');
        I->add_edge(RI, ' ');
      }
      // If RF has no out-edges then we can add an epsilon transition from LI=LF to RF instead of creating a new node
      if ( RF -> num_out_edges() == 0 ) {
        LF->add_edge(RF, ' ');
        F = RF;
      } else {
        F = new Node();
        LF->add_edge(F, ' ');
        RF->add_edge(F, ' ');
      }
      return ThompsonsConstruction(I, F);
    }
    if ( RI == RF ) {
      // If LI has no in-edges then we can add an epsilon transition from LI to RI=RF instead of creating a new node
      if ( LI -> num_in_edges() == 0 ) {
        LI->add_edge(RI, ' ');
        I = LI;
      } else {
        I = new Node();
        I->add_edge(LI, ' ');
        I->add_edge(RI, ' ');
      }
      // If LF has no out-edges then we can add an epsilon transition from RI=RF to LF instead of creating a new node
      if ( LF -> num_out_edges() == 0 ) {
        RF->add_edge(LF, ' ');
        F = LF;
      } else {
        F = new Node();
        LF->add_edge(F, ' ');
        RF->add_edge(F, ' ');
      }
      return ThompsonsConstruction(I, F);
    }

    // TODO: are there any other LI==LF or RI==RF improvements?

    if ( LI == LF || RI == RF ) throw std::logic_error("ThompsonsConstruction::alternation. Unexpected case.\n");

    if (have_same_self_edges (LI, RI) && 
        LI->num_nonself_in_edges() == 0 &&
        RI->num_nonself_in_edges() == 0 ) {
      // std::cout << " merging initial\n";
      I = LI;
      RI -> redirect(I);
    } else {
      I = new Node();
      I->add_edge(LI, ' ');
      I->add_edge(RI, ' ');
    }
    if (have_same_self_edges (LF, RF) &&
        LF->num_nonself_out_edges() == 0 && 
        RF->num_nonself_out_edges() == 0 ) {
      // std::cout << " merging final\n";
      F = RF;
      LF -> redirect(F);
      return ThompsonsConstruction(I, F);
    } else {
      F = new Node();
      LF -> add_edge(F, ' ');
      RF -> add_edge(F, ' ');
      return ThompsonsConstruction(I, F);
    }
  }

  inline ThompsonsConstruction
  concatenation ( ThompsonsConstruction && lhs, ThompsonsConstruction && rhs) {
    // std::cout << "concatenation\n";
    if (lhs.final()->num_out_edges() == 0 || 
        rhs.initial()->num_in_edges() == 0 || 
        (lhs.final()->num_nonself_out_edges() == 0 && rhs.initial()->num_nonself_in_edges() == 0 && have_same_self_edges (lhs.final(), rhs.initial() ) ) ) {
      auto I = lhs.initial();
      auto F = rhs.final();
      lhs.final() -> redirect(rhs.initial());
      if (lhs.initial() == lhs.final() ) I = rhs.initial();
      return ThompsonsConstruction(I, F);
    } else {
      auto I = lhs.initial();
      auto F = rhs.final();
      lhs.final() -> add_edge(rhs.initial(), ' ');
      return ThompsonsConstruction(I, F);
    }
  }

  inline ThompsonsConstruction
  kleene ( ThompsonsConstruction && nfa ) {
    // std::cout << "kleene\n";
    if ( nfa.initial() == nfa.final() ) {
      return ThompsonsConstruction(nfa.initial(), nfa.final());
    }
    if ( nfa.initial()->num_in_edges() == 0 && nfa.final()->num_out_edges() == 0 ) {
      nfa.final() -> redirect(nfa.initial());
      return ThompsonsConstruction(nfa.initial(), nfa.initial());
    }
    return question(plus(std::move(nfa)));
  }

  inline ThompsonsConstruction
  plus ( ThompsonsConstruction && nfa ) {
    // one or more times
    // std::cout << "plus\n";
    if ( nfa.initial() == nfa.final() ) {
      return ThompsonsConstruction(nfa.initial(), nfa.final());
    }
    nfa.final()->add_edge(nfa.initial(), ' ');
    return ThompsonsConstruction(nfa.initial(), nfa.final());
  }

  inline ThompsonsConstruction
  question ( ThompsonsConstruction && nfa ) {
    // note: can't simplify by making new final same as old final because then mulitiple paths would match same string
    // std::cout << "question\n";
    if ( nfa.initial() == nfa.final() ) {
      return ThompsonsConstruction(nfa.initial(), nfa.final());
    }
    if ( nfa.initial()->num_in_edges() == 0 && nfa.final()->num_out_edges() == 0 ) {
      nfa.initial()->add_edge(nfa.final(),' ');
      return ThompsonsConstruction(nfa.initial(), nfa.final());
    }
    if ( nfa.initial()->num_in_edges() == 0 ) {
      auto F = new Node();
      nfa.initial()->add_edge(F, ' ');
      nfa.final()->add_edge(F, ' ');
      return ThompsonsConstruction(nfa.initial(), F);
    }
    if ( nfa.final()->num_out_edges() == 0 ) {
      auto I = new Node();
      I->add_edge(nfa.initial(), ' ');
      I->add_edge(nfa.final(), ' ');
      return ThompsonsConstruction(I, nfa.final());
    }  
    auto I = new Node();
    auto F = new Node();
    I->add_edge(nfa.initial(), ' ');
    I->add_edge(F, ' ');
    nfa.final()->add_edge(F, ' ');
    return ThompsonsConstruction(I, F);
  }


  inline
  ThompsonsConstruction::ThompsonsConstruction ( void ) {}

  inline
  ThompsonsConstruction::ThompsonsConstruction ( char c ) {
    initial_ = new Node();
    final_ = new Node();
    initial_->add_edge(final_, c);
  }

  inline
  ThompsonsConstruction::ThompsonsConstruction ( Node * I, Node * F ) : initial_(I), final_(F) {}

  inline
  Node *
  ThompsonsConstruction::initial ( void ) const {
    return const_cast<Node *>(initial_);
  }

  inline
  Node *
  ThompsonsConstruction::final ( void ) const {
    return const_cast<Node *>(final_);
  }

  inline std::unordered_set<Node *>
  ThompsonsConstruction::nodes ( void ) {
    std::unordered_set<Node *> explored;
    std::stack<Node *> nodes_;
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
  ThompsonsConstruction::delete_nodes ( void ) {
    for ( auto n : nodes() ) delete n;
    return; 
  }

  inline void 
  ThompsonsConstruction::contract ( void ) {
    for ( auto n : nodes() ) {
      if ( n != initial() && n != final() ) {
        n -> simplify();
      }
    }
    return;
  }

}


inline 
NFA
CompileRegexToNFA (std::string const& regex) {
  using namespace ThompsonsConstruction_detail;
  /// Parse regex between i and j
  typedef std::pair<int,int> Term;
  std::function<ThompsonsConstruction(Term)> parse = [&](Term t) {
    // Algorithm. Precedence is parentheses, kleene star, then concatenation, then alternation.
    // Step 0. If single character result basic result.
    // Step 1. Find all top level terms. (parentheses pairs or single characters) Recurse.
    // Step 2. For each pair, check if the term is kleene starred. 
    // Step 3. Concatenate consecutive terms between | bars
    // Step 4. Combine concatenations with alternation
    int i = t.first;
    int j = t.second;
    if ( j == i + 1 ) return ThompsonsConstruction(regex[i]); // Single character
    std::vector<std::vector<ThompsonsConstruction>> expression; // alternations of concatenations
    std::vector<ThompsonsConstruction> concatenation_string;
    while ( i < j ) {
      if (regex[i] == '*' || regex[i] == '+' || regex[i] == '?') {
        if ( concatenation_string.size() == 0 ) throw std::invalid_argument("ThompsonsConstruction constructor: invalid regex " + regex);
        ThompsonsConstruction back = concatenation_string.back();
        concatenation_string.pop_back();
        switch ( regex[i] ) {
          case '*': back = kleene(std::move(back)); break;
          case '+': back = plus(std::move(back)); break;
          case '?': back = question(std::move(back)); break;
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
          throw std::invalid_argument("ThompsonsConstruction constructor: invalid regex " + regex );
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
    std::vector<ThompsonsConstruction> alternation_string;
    for ( auto cs : expression ) {
      if ( cs.size() == 0 ) continue;
      ThompsonsConstruction nfa = cs[0];
      for ( int i = 1; i < cs.size(); ++ i ) {
        nfa = concatenation(std::move(nfa), std::move(cs[i]));
      }
      alternation_string.push_back(nfa);
    }
    // perform alternations
    if ( alternation_string.size() == 0 ) throw std::invalid_argument("ThompsonsConstruction constructor: invalid regex " + regex);
    ThompsonsConstruction result = alternation_string[0];
    for ( int i = 1; i < alternation_string.size(); ++ i ) {
      result = alternation(std::move(result), std::move(alternation_string[i]));
    }
    return result;
  };
  auto result = parse({0, regex.size()});
  result.contract();

  NFA nfa;

  std::unordered_map<Node *, uint64_t> node_index;
  for ( auto n : result.nodes() ) { 
    node_index[n] = nfa.add_vertex();
    nfa.add_edge(node_index[n], node_index[n], ' '); // self-epsilon transitions
  }
  std::unordered_set<uint64_t> explored;
  std::stack<Node *> nodes_;
  nodes_.push(result.initial());
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
        nfa.add_edge(i,j,label);
        // std::cout << "added.\n";
      }
    }
  }
  // std::cout << "finalize while loop complete.\n";
  nfa.set_initial(node_index[result.initial()]);
  nfa.set_final(node_index[result.final()]);
  for ( auto n : result.nodes () ) delete n;
  return nfa;
}
