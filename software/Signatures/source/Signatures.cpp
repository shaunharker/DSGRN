/// Main algorithm

#include "Signatures.h"
#include "delegator/delegator.h" 
#include "Database/sqlambda.h"
#include "Parameter/Network.h"
#include "Parameter/Parameter.h"
#include "Parameter/ParameterGraph.h"
#include "Phase/DomainGraph.h"
#include "Dynamics/MorseDecomposition.h"
#include "Dynamics/MorseGraph.h"

using namespace sqlite;

int main ( int argc, char * argv [] ) {
  if ( argc < 4 ) {
    std::cout << "Please supply the following arguments:\n" 
                  " --> network specification file \n"
                  " --> logic data folder \n"
                  " --> output folder \n";
    return 1;
  }
  delegator::Start ();
  delegator::Run < Signatures > (argc, argv);
  delegator::Stop ();
  return 0;
}

void Signatures::
command_line ( int argc, char * argv [] ) {
  network_spec_filename_ = argv[1];
  logic_folder_ = argv[2];
  database_filename_ = argv[3];
  // Load the network file and initialize the parameter graph
  std::shared_ptr<Network> network ( new Network );
  network -> load ( network_spec_filename_ );
  pg_ . assign ( network, logic_folder_ );
  std::cout << "Parameter Graph size = " << pg_ . size () << "\n";
}

void Signatures::
initialize ( void ) {
  // Open the database
  db_ = database ( database_filename_ );

  // Create an SQLite database
  db_ . exec ( "create table if not exists Signatures (ParameterIndex INTEGER PRIMARY KEY, MorseGraphIndex INTEGER);" );
  //db_ . exec ( "create table if not exists MorseGraphSHA (MorseGraphIndex INTEGER PRIMARY KEY, SHA TEXT);" );
  db_ . exec ( "create table if not exists MorseGraphViz (MorseGraphIndex INTEGER PRIMARY KEY, Graphviz TEXT);" );
  db_ . exec ( "create table if not exists MorseGraphVertices (MorseGraphIndex INTEGER, Vertex INTEGER);" );
  db_ . exec ( "create table if not exists MorseGraphEdges (MorseGraphIndex INTEGER, Source INTEGER, Target INTEGER);" );
  db_ . exec ( "create table if not exists MorseGraphAnnotations (MorseGraphIndex INTEGER, Vertex INTEGER, Label TEXT);" );

  current_job_ = 0;
}

int  Signatures::
prepare ( Message & job ) {
  //std::cout << "Signatures::prepare\n";
  //std::cout << "Parameter Graph size = " << pg_ . size () << "\n";
  std::cout << "Current job # = " << current_job_ << "\n";
  // Prepare a job
  std::vector<uint64_t> parameter_indices;
  while ( parameter_indices . size () < 1024 &&
          current_job_ < pg_ . size () ) {
    parameter_indices . push_back ( current_job_ ++ );
  }
  if ( parameter_indices . empty () ) return 1; // All done!
  job << parameter_indices;
  return 0; // Job sent!
}

void Signatures::
work ( Message & result, const Message & job ) const {
  // Perform the work
  std::vector<uint64_t> parameter_indices;
  job >> parameter_indices;
  typedef std::pair<uint64_t, std::shared_ptr<MorseGraph>> Result_t;
  std::vector<Result_t> result_data;
  for ( uint64_t pi : parameter_indices ) {
    std::shared_ptr<Parameter> param = pg_ . parameter ( pi );
    std::shared_ptr<DomainGraph> dg ( new DomainGraph );
    dg -> assign ( param );
    std::shared_ptr<MorseDecomposition> md ( new MorseDecomposition );
    md -> assign ( dg -> digraph () );
    std::shared_ptr<MorseGraph> mg ( new MorseGraph );
    mg -> assign ( dg, md );
    Result_t r ( pi,  mg );
    result_data . push_back ( r );
  }
  result << result_data;
}

void Signatures::
accept ( const Message &result ) {
  // Accept the results
  typedef std::pair<uint64_t, std::shared_ptr<MorseGraph>> Result_t;
  std::vector<Result_t> result_data;
  result >> result_data;

  //std::cout << "Signatures::accept\n";
  //std::cout << "Received " << result_data . size () << " results.\n";
  // Begin a transaction
  //std::cout << "Beginning SQLite transaction.\n";
  db_ . exec ( "begin;" );
  // Insert the Morse Graphs into the database (or get indices)
  //std::cout << "Inserting Morse Graphs\n";
  std::vector<uint64_t> mg_indices;
  for ( Result_t const& r : result_data ) {
    mg_indices . push_back ( insertMorseGraph ( r . second ) );
  }
  // Insert the signatures
  //std::cout << "Inserting Signatures\n";
  statement prepped = db_ . prepare ( "insert into Signatures (ParameterIndex, MorseGraphIndex) values (?, ?);" );
  for ( uint64_t k = 0; k < mg_indices . size (); ++ k ) {
    prepped . bind ( result_data[k] . first, mg_indices[k] ) . exec ();
  }
  // Commit the transaction
  //std::cout << "Committing SQLite transaction.\n";
  db_ . exec ( "end;" );
}

void Signatures::
finalize ( void ) {
  // Create the indices
  db_ . exec ( "create index if not exists Signatures2 on Signatures (MorseGraphIndex, ParameterIndex);");
  db_ . exec ( "create index if not exists MorseGraphAnnotations3 on MorseGraphAnnotations (Label, MorseGraphIndex);");
  //db_ . exec ( "create index if not exists MorseGraphSHA2 on MorseGraphSHA (SHA);");
  db_ . exec ( "create index if not exists MorseGraphViz2 on MorseGraphViz (Graphviz, MorseGraphIndex);");

  // Are these necessary?
  db_ . exec ( "create index if not exists MorseGraphVertices1 on MorseGraphVertices (MorseGraphIndex, Vertex);");
  db_ . exec ( "create index if not exists MorseGraphVertices2 on MorseGraphVertices (Vertex, MorseGraphIndex);");
  db_ . exec ( "create index if not exists MorseGraphEdges1 on MorseGraphEdges (MorseGraphIndex);");
  db_ . exec ( "create index if not exists MorseGraphAnnotations1 on MorseGraphAnnotations (MorseGraphIndex);");

  // Close the database
  // happens on exit
}

uint64_t Signatures::
insertMorseGraph ( std::shared_ptr<MorseGraph> mg ) {
  std::stringstream ss;
  ss << * mg;
  std::string gv = ss . str ();
  if ( mg_lookup_ . count ( gv ) ) return mg_lookup_ [ gv ];
  uint64_t mgi = mg_lookup_ . size ();
  mg_lookup_ [ gv ] = mgi;
  statement prepped;
  //std::cout << "Inserting into MorseGraphViz\n";
  prepped = db_ . prepare ( "insert into MorseGraphViz (MorseGraphIndex, Graphviz) values (?, ?);" );
  prepped . bind ( mgi, gv ) . exec ();
  //std::cout << "Inserting into MorseGraphVertices\n";
  prepped = db_ . prepare ( "insert into MorseGraphVertices (MorseGraphIndex, Vertex) values (?, ?);" );
  uint64_t N = mg -> poset () -> size ();
  for ( uint64_t v = 0; v < N; ++ v ) prepped . bind ( mgi, v ) . exec ();
  //std::cout << "Inserting into MorseGraphEdges\n";
  prepped = db_ . prepare ( "insert into MorseGraphEdges (MorseGraphIndex, Source, Target) values (?, ?, ?);" );
  for ( uint64_t source = 0; source < N; ++ source ) { 
    for ( uint64_t target : mg -> poset () -> adjacencies ( source ) ) {
      prepped . bind ( mgi, source, target ) . exec ();
    }
  }
  //std::cout << "Inserting into MorseGraphAnnotations\n";
  prepped = db_ . prepare ( "insert into MorseGraphAnnotations (MorseGraphIndex, Vertex, Label) values (?, ?, ?);" );
  for ( uint64_t v = 0; v < N; ++ v ) { 
    Annotation const& a = mg -> annotation ( v );
    for ( std::string const& label : a ) { 
      prepped . bind ( mgi, v, label ) . exec ();
    }
  }
  return mgi;
}

/*
uint64_t Signatures::
insertMorseGraph ( std::shared_ptr<MorseGraph> mg ) {
  std::string sha = mg -> SHA256 ();
  if ( mg_lookup_ . count ( sha ) ) return mg_lookup_ [ sha ];
  uint64_t mgi = mg_lookup_ . size ();
  mg_lookup_ [ sha ] = mgi;
  statement prepped;
  //std::cout << "Inserting into MorseGraphSHA\n";
  prepped = db_ . prepare ( "insert into MorseGraphSHA (MorseGraphIndex, SHA) values (?, ?);" );
  prepped . bind ( mgi, sha ) . exec ();
  //std::cout << "Inserting into MorseGraphVertices\n";
  prepped = db_ . prepare ( "insert into MorseGraphVertices (MorseGraphIndex, Vertex) values (?, ?);" );
  uint64_t N = mg -> poset () -> size ();
  for ( uint64_t v = 0; v < N; ++ v ) prepped . bind ( mgi, v ) . exec ();
  //std::cout << "Inserting into MorseGraphEdges\n";
  prepped = db_ . prepare ( "insert into MorseGraphEdges (MorseGraphIndex, Source, Target) values (?, ?, ?);" );
  for ( uint64_t source = 0; source < N; ++ source ) { 
    for ( uint64_t target : mg -> poset () -> adjacencies ( source ) ) {
      prepped . bind ( mgi, source, target ) . exec ();
    }
  }
  //std::cout << "Inserting into MorseGraphAnnotations\n";
  prepped = db_ . prepare ( "insert into MorseGraphAnnotations (MorseGraphIndex, Vertex, Label) values (?, ?, ?);" );
  for ( uint64_t v = 0; v < N; ++ v ) { 
    Annotation const& a = mg -> annotation ( v );
    for ( std::string const& label : a ) { 
      prepped . bind ( mgi, v, label ) . exec ();
    }
  }
  return mgi;
}
*/
