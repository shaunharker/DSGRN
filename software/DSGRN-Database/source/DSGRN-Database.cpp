/// Main algorithm

#include "DSGRN-Database.h"

using namespace sqlite;

int main ( int argc, char * argv [] ) {
  if ( argc < 3 ) {
    std::cout << "Please supply the following arguments:\n" 
                  " --> network specification file \n"
                  " --> output file \n"
                  " --> [start parameter index] (optional)\n"
                  " --> [one-past-end parameter index] (optional)\n";
    return 1;
  }
  Signatures process;
  if ( process . command_line ( argc, argv ) ) return 1;
  process . initialize ();
  process . mainloop ();
  process . finalize ();
  return 0;
}

int Signatures::
command_line ( int argc, char * argv [] ) {
  network_spec_filename_ = argv[1];
  database_filename_ = argv[2];

  // Load the network file and initialize the parameter graph
  network_ . load ( network_spec_filename_ );
  pg_ . assign ( network_ );
  std::cout << "Parameter Graph size = " << pg_ . size () << "\n";

  if ( argc == 3 ) {
    start_job_ = 0;
    end_job_ = pg_ . size ();
  } else {
    if ( argc < 5 ) { 
      return 1;
    }
    start_job_ = std::stoull(argv[3]);
    end_job_ = std::stoull(argv[4]);
  }
  return 0;
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

  // Create Network metadata

  // Get Network Name from database filename
  auto lastslash = database_filename_.find_last_of("/"); 
  auto lastdot = database_filename_.find_last_of("."); 
  if ( lastslash == std::string::npos ) lastslash = 0; else ++ lastslash;
  if ( lastdot == std::string::npos ) lastdot = database_filename_ . size ();
  std::string network_name = database_filename_.substr(lastslash, lastdot - lastslash); 

  db_ . exec ( "create table if not exists Network ( Name TEXT, Dimension INTEGER, Specification TEXT, Graphviz TEXT);");
  statement prepped = db_ . prepare ( "insert into Network (Name, Dimension, Specification, Graphviz) values (?, ?, ?, ?);");
  prepped . bind ( network_name, network_ . size(), network_ . specification (), network_ . graphviz () ) . exec ();
}

void Signatures::mainloop ( void ) {

  // Prepare statements
  statement InsertIntoMorseGraphViz = db_ . prepare ( "insert into MorseGraphViz (MorseGraphIndex, Graphviz) values (?, ?);" );
  statement InsertIntoMorseGraphVertices = db_ . prepare ( "insert into MorseGraphVertices (MorseGraphIndex, Vertex) values (?, ?);" );
  statement InsertIntoMorseGraphEdges = db_ . prepare ( "insert into MorseGraphEdges (MorseGraphIndex, Source, Target) values (?, ?, ?);" );
  statement InsertIntoMorseGraphAnnotations = db_ . prepare ( "insert into MorseGraphAnnotations (MorseGraphIndex, Vertex, Label) values (?, ?, ?);" );
  statement InsertIntoSignatures = db_ . prepare ( "insert into Signatures (ParameterIndex, MorseGraphIndex) values (?, ?);" );

  // Begin a transaction
  db_ . exec ( "begin;" );
  
  ///////////////
  // main loop //
  ///////////////
  for ( uint64_t pi = start_job_; pi < end_job_; ++ pi ) {
    //////////
    // work //
    //////////
    Parameter param = pg_ . parameter ( pi );
    DomainGraph dg ( param );
    MorseDecomposition md ( dg . digraph () );
    MorseGraph mg ( dg, md );
    
    ////////////////////////
    // Insert Morse Graph //
    ////////////////////////
    uint64_t mgi;
    std::stringstream ss;
    ss << mg;
    std::string gv = ss . str ();
    if ( mg_lookup_ . count ( gv ) ) { 
      mgi = mg_lookup_ [ gv ];
    } else {
      mgi = mg_lookup_ . size ();
      mg_lookup_ [ gv ] = mgi;
      InsertIntoMorseGraphViz . bind ( mgi, gv ) . exec ();
      uint64_t N = mg . poset () . size ();
      for ( uint64_t v = 0; v < N; ++ v ) { 
        InsertIntoMorseGraphVertices . bind ( mgi, v ) . exec ();
      }
      for ( uint64_t source = 0; source < N; ++ source ) { 
        for ( uint64_t target : mg . poset () . children ( source ) ) {
          InsertIntoMorseGraphEdges . bind ( mgi, source, target ) . exec ();
        }
      }
      for ( uint64_t v = 0; v < N; ++ v ) { 
        Annotation const& a = mg . annotation ( v );
        for ( std::string const& label : a ) { 
          InsertIntoMorseGraphAnnotations . bind ( mgi, v, label ) . exec ();
        }
      }
    }
    //////////////////////
    // Insert signature //
    //////////////////////
    InsertIntoSignatures . bind ( pi, mgi ) . exec ();
  }
  // End the transaction
  db_ . exec ( "end;" );
}

void Signatures::
finalize ( void ) {
  // Create the indices
  db_ . exec ( "create index if not exists Signatures2 on Signatures (MorseGraphIndex, ParameterIndex);");
  db_ . exec ( "create index if not exists MorseGraphAnnotations3 on MorseGraphAnnotations (Label, MorseGraphIndex);");
  db_ . exec ( "create index if not exists MorseGraphViz2 on MorseGraphViz (Graphviz, MorseGraphIndex);");
  db_ . exec ( "create index if not exists MorseGraphVertices1 on MorseGraphVertices (MorseGraphIndex, Vertex);");
  db_ . exec ( "create index if not exists MorseGraphVertices2 on MorseGraphVertices (Vertex, MorseGraphIndex);");
  db_ . exec ( "create index if not exists MorseGraphEdges1 on MorseGraphEdges (MorseGraphIndex);");
  db_ . exec ( "create index if not exists MorseGraphAnnotations1 on MorseGraphAnnotations (MorseGraphIndex);");
}
