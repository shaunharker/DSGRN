/// NumericalSimulationPatternMatch.cpp
/// Shaun Harker
/// 2016-04-20

#include "NumericalSimulationPatternMatch.h"

#define COHORT_SIZE 256

using namespace sqlite;

std::string help_string = 
  "Please supply the following arguments:\n" 
  " --> network specification file \n"
  " --> pattern specification file \n"
  " --> analysis file: contains on each line a JSON string encoding an object\n"
  "                    {\"ParameterIndex\":...,\"SimulationSummary\":[...],...any other fields...}\n"
  " --> output file (into which will be stored lines from analysis file which match pattern)\n";

int main ( int argc, char * argv [] ) {
  if ( argc < 3 ) {
    std::cout << help_string;
    return 1;
  }
  delegator::Start ();
  delegator::Run < NumericalSimulationPatternMatch > (argc, argv);
  delegator::Stop ();
  return 0;
}

void NumericalSimulationPatternMatch::
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

void NumericalSimulationPatternMatch::
initialize ( void ) {
  // Initialize input and output streams
  input_file_ . open ( input_filename_ );
  output_file_ . open ( output_filename_ );
  jobs_submitted_ = 0;
}

int NumericalSimulationPatternMatch::
prepare ( Message & job ) {
  // Prepare a job
  //   The job is a list of parameter index, morse set index pairs
  //   (They are submitted in groups rather than one at a time to
  //    reduce communication overhead)
  std::vector<std::string> job_cohort;
  while ( job_cohort . size () < COHORT_SIZE ) {
    std::string line;
    if ( not std::getline(input_file_, line) ) break;
    job_cohort . push_back ( line );
  }
  if ( job_cohort . empty () ) return 1; // All done!
  job << job_cohort;
  jobs_submitted_ += job_cohort . size ();
  std::cout << "Submitted " << jobs_submitted_ << " jobs so far.\n";
  return 0; // Job sent!
}

void NumericalSimulationPatternMatch::
work ( Message & result, const Message & job ) const {
  std::vector<std::string> job_cohort;
  job >> job_cohort;
  std::vector<std::string> result_data;
  // Loop through (parameter_index, morse_set_index) pairs and 
  // filter out those for which there is no match
  for ( auto const& line : job_cohort ) {
    //std::cout << "Processing " << line << "\n";
    json j = json::parse(line);
    std::vector<uint64_t> labels;
    for ( uint64_t label : j["SimulationSummary"] ) labels . push_back ( label );
    SearchGraph searchgraph ( labels, network_ . size () );
    MatchingGraph matchinggraph ( searchgraph, patterngraph_ );
    if ( QueryPathMatch ( matchinggraph ) ) result_data . push_back ( line );
  }
  result << result_data;
}

void NumericalSimulationPatternMatch::
accept ( const Message &result ) {
  std::vector<std::string> result_data;
  result >> result_data;
  for ( auto const& line : result_data ) {
    output_file_ << line << "\n";
  }
}

void NumericalSimulationPatternMatch::
finalize ( void ) {
  input_file_ . close ();
  output_file_ . close ();
  std::cout << "Finished.\n";
}
