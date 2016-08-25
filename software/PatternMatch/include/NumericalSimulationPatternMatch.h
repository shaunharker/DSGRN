/// NumericalSimulationPatternMatch.h
/// Shaun Harker
/// 2016-04-20

#ifndef DSGRN_NUMERICALSIMULATIONPATTERNMATCH_H
#define DSGRN_NUMERICALSIMULATIONPATTERNMATCH_H

#include "common.h"
#include "cluster-delegator.hpp"
#include "Tools/sqlambda.h"
#include "DSGRN.h"
#include "PatternMatch.h"

class NumericalSimulationPatternMatch : public Coordinator_Worker_Process {
 public:
  void command_line ( int argc, char * argv [] );
  void initialize ( void );
  int  prepare ( Message & job );
  void work ( Message & result, const Message & job ) const;
  void accept ( const Message &result );
  void finalize ( void ); 
 private:
  std::string network_spec_filename_;
  std::string pattern_spec_filename_;
  std::string input_filename_;
  std::string output_filename_;
  Network network_;
  ParameterGraph parametergraph_;
  Pattern pattern_;
  PatternGraph patterngraph_;
  std::ifstream input_file_;
  std::ofstream output_file_;
  uint64_t jobs_submitted_;
};
#endif
