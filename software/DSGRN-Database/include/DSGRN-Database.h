/// DSGRN-Database.h
/// Shaun Harker
/// 2015-08-23

#ifndef DSGRN_DATABASE_H
#define DSGRN_DATABASE_H

#include "common.h"
#include "Tools/sqlambda.h"
#include "DSGRN.h"

class Signatures {
public:
  int command_line ( int argc, char * argv [] );
  void initialize ( void );
  void mainloop ( void );
  void finalize ( void ); 
private:
  std::string network_spec_filename_;
  std::string database_filename_;
  ParameterGraph pg_;
  Network network_;
  uint64_t start_job_;
  uint64_t end_job_;
  sqlite::database db_;
  std::unordered_map<std::string, uint64_t> mg_lookup_;
};
#endif
