/// ParameterSignatureProcess.h
/// Shaun Harker
/// 2015-05-20

#ifndef DSGRN_PARAMETERSIGNATUREPROCESS_H
#define DSGRN_PARAMETERSIGNATUREPROCESS_H

#include "common.h"
#include "cluster-delegator.hpp"
#include "Tools/sqlambda.h"
#include "DSGRN.h"

class Signatures : public Coordinator_Worker_Process {
public:
  void command_line ( int argc, char * argv [] );
  void initialize ( void );
  int  prepare ( Message & job );
  void work ( Message & result, const Message & job ) const;
  void accept ( const Message &result );
  void finalize ( void ); 
private:
  std::string network_spec_filename_;
  std::string logic_folder_;
  std::string database_filename_;
  ParameterGraph pg_;
  Network network_;
  uint64_t current_job_;
  sqlite::database db_;
  std::unordered_map<std::string, uint64_t> mg_lookup_;
  uint64_t insertMorseGraph ( MorseGraph const& mg );
};
#endif
