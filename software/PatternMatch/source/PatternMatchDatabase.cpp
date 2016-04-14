/// PatternMatchDatabase.cpp
/// Shaun Harker and Bree Cummins
/// 2016-03-29

#include "PatternMatchDatabase.h"

#define COHORT_SIZE 256

using namespace sqlite;

std::string help_string = 
  "Please supply the following arguments:\n" 
  " --> network specification file \n"
  " --> pattern specification file \n"
  " --> analysis file: contains on each line parameter node index, morse set index, e.g.\n"
  "                    12 3\n"
  "                    12 4\n"
  "                    288 1\n"
  "                    ...\n"
  " --> output file \n";

int main ( int argc, char * argv [] ) {
  if ( argc < 3 ) {
    std::cout << help_string;
    return 1;
  }
  delegator::Start ();
  delegator::Run < PatternMatchDatabase > (argc, argv);
  delegator::Stop ();
  return 0;
}

void PatternMatchDatabase::
command_line ( int argc, char * argv [] ) {
  network_spec_filename_ = argv[1];
  pattern_spec_filename_ = argv[2];
  input_filename_ = argv[3];
  output_filename_ = argv[4];
  // Load the network file and initialize the parameter graph
  network_ . load ( network_spec_filename_ );
  pattern_ . load ( pattern_spec_filename_ );
  patterngraph_ . assign ( pattern_ );
  parametergraph_ . assign ( network_ );
  std::cout << "Parameter Graph size = " << parametergraph_ . size () << "\n";
}

void PatternMatchDatabase::
initialize ( void ) {
  // Initialize input and output streams
  input_file_ . open ( input_filename_ );
  output_file_ . open ( output_filename_ );
  jobs_submitted_ = 0;
}

int  PatternMatchDatabase::
prepare ( Message & job ) {
  // Prepare a job
  //   The job is a list of parameter index, morse set index pairs
  //   (They are submitted in groups rather than one at a time to
  //    reduce communication overhead)
  std::vector<std::pair<uint64_t,uint64_t>> job_cohort;
  while ( job_cohort . size () < COHORT_SIZE ) {
    uint64_t parameter_index;
    uint64_t morse_set_index;
    input_file_ >> parameter_index >> morse_set_index;
    if ( not input_file_ . good () ) break;
    job_cohort . push_back ( {parameter_index, morse_set_index} );
  }
  if ( job_cohort . empty () ) return 1; // All done!
  job << job_cohort;
  jobs_submitted_ += job_cohort . size ();
  std::cout << "Submitted " << jobs_submitted_ << " jobs so far.\n";
  return 0; // Job sent!
}

void PatternMatchDatabase::
work ( Message & result, const Message & job ) const {
  std::vector<std::pair<uint64_t,uint64_t>> job_cohort;
  job >> job_cohort;
  std::vector<std::pair<uint64_t,uint64_t>> result_data;
  // Loop through (parameter_index, morse_set_index) pairs and 
  // filter out those for which there is no match
  for ( auto const& pair : job_cohort ) {
    uint64_t parameter_index = pair.first;
    uint64_t morse_set_index = pair.second;
    Parameter p = parametergraph_ . parameter ( parameter_index );
    DomainGraph domaingraph ( p );
    SearchGraph searchgraph ( domaingraph, morse_set_index );
    MatchingGraph matchinggraph ( searchgraph, patterngraph_ );
    if ( QueryCycleMatch ( matchinggraph ) ) result_data . push_back ( pair );
  }
  result << result_data;
}

void PatternMatchDatabase::
accept ( const Message &result ) {
  std::vector<std::pair<uint64_t,uint64_t>> result_data;
  result >> result_data;
  for ( auto const& pair : result_data ) {
    output_file_ << pair.first << " " << pair.second << "\n";
  }
}

void PatternMatchDatabase::
finalize ( void ) {
  input_file_ . close ();
  output_file_ . close ();
  std::cout << "Finished.\n";
}
