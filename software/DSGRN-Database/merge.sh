#!/bin/bash
# Merge script
# ./merge.sh folder target
# Merges all DSGRN databases in folder 
# into a single database named target

# Initialize (erase target if already exists)
folder=$1
target=$2
rm -rf $target

# Index database shards
echo 'Indexing database shards...'
for db in `ls $folder`; do
    echo $db
    echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
    echo "create index if not exists MorseGraphVizAsc on MorseGraphViz (Graphviz ASC, MorseGraphIndex);" >> commands.sql
    cat commands.sql | sqlite3 $folder/$db
    rm commands.sql
done

# Create List of all Morse Graphs (in graphviz form)
echo 'Scanning database shards...'
sqlite3 $target 'create table MGList (Graphviz TEXT PRIMARY KEY ASC);'
for db in `ls $folder`; do
    echo $db
    echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
    echo "attach '$folder/$db' as mergeMe;" >> commands.sql
    echo "explain query plan insert or ignore into MGList select Graphviz from mergeMe.MorseGraphViz order by Graphviz;" >> commands.sql
    echo "insert or ignore into MGList select Graphviz from mergeMe.MorseGraphViz order by Graphviz;" >> commands.sql
    cat commands.sql | sqlite3 $target
    rm commands.sql
done

# Enumerate Morse graphs (i.e. give MorseGraphIndex field)
# and initialize database
echo 'Initializing database'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'create table xMorseGraphViz (MorseGraphIndex INTEGER PRIMARY KEY, Graphviz TEXT);' >> commands.sql
echo 'insert or ignore into xMorseGraphViz select rowid as MorseGraphIndex,Graphviz from MGList;' >> commands.sql
echo 'drop table MGList;' >> commands.sql
echo 'create table xMorseGraphVertices (MorseGraphIndex INTEGER, Vertex INTEGER, unique(MorseGraphIndex,Vertex));' >> commands.sql
echo 'create table xMorseGraphEdges (MorseGraphIndex INTEGER, Source INTEGER, Target INTEGER, unique(MorseGraphIndex,Source,Target));' >> commands.sql
echo 'create table xMorseGraphAnnotations (MorseGraphIndex INTEGER, Vertex INTEGER, Label TEXT, unique(MorseGraphIndex,Vertex,Label));' >> commands.sql
echo 'create table xSignatures (ParameterIndex INTEGER PRIMARY KEY, MorseGraphIndex INTEGER);' >> commands.sql
echo 'create table xNetwork ( Name TEXT PRIMARY KEY, Dimension INTEGER, Specification TEXT, Graphviz TEXT);' >> commands.sql
echo "create index if not exists Signatures2 on xSignatures (MorseGraphIndex, ParameterIndex);" >> commands.sql
echo "create index if not exists MorseGraphAnnotations3 on xMorseGraphAnnotations (Label, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphViz2 on xMorseGraphViz (Graphviz, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphVertices1 on xMorseGraphVertices (MorseGraphIndex, Vertex);" >> commands.sql
echo "create index if not exists MorseGraphVertices2 on xMorseGraphVertices (Vertex, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphEdges1 on xMorseGraphEdges (MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphAnnotations1 on xMorseGraphAnnotations (MorseGraphIndex);" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql

echo 'Merging database shards...'

# Merge databases
for db in `ls $folder`; do
    echo ' ---------------------- '
    echo $db
    echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
    echo "attach '$folder/$db' as mergeMe;" >> commands.sql
    
    echo 'explain query plan create table Map as select S.MorseGraphIndex as source,T.MorseGraphIndex as target from (mergeMe.MorseGraphViz as S join xMorseGraphViz as T on S.Graphviz=T.Graphviz);' >> commands.sql    
    echo 'create table Map as select S.MorseGraphIndex as source,T.MorseGraphIndex as target from (mergeMe.MorseGraphViz as S join xMorseGraphViz as T on S.Graphviz=T.Graphviz);' >> commands.sql
    echo 'create index Map1 on Map (source, target)'

    echo 'explain query plan insert or ignore into xMorseGraphVertices select Map.target as MorseGraphIndex, Vertex from mergeMe.MorseGraphVertices join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'insert or ignore into xMorseGraphVertices select Map.target as MorseGraphIndex, Vertex from mergeMe.MorseGraphVertices join Map where MorseGraphIndex=Map.source;' >> commands.sql

    echo 'explain query plan insert or ignore into xMorseGraphEdges select Map.target as MorseGraphIndex, mergeMe.MorseGraphEdges.Source, mergeMe.MorseGraphEdges.Target from mergeMe.MorseGraphEdges join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'insert or ignore into xMorseGraphEdges select Map.target as MorseGraphIndex, mergeMe.MorseGraphEdges.Source, mergeMe.MorseGraphEdges.Target from mergeMe.MorseGraphEdges join Map where MorseGraphIndex=Map.source;' >> commands.sql

    echo 'explain query plan insert or ignore into xMorseGraphAnnotations select Map.target as MorseGraphIndex, Vertex, Label from mergeMe.MorseGraphAnnotations join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'insert or ignore into xMorseGraphAnnotations select Map.target as MorseGraphIndex, Vertex, Label from mergeMe.MorseGraphAnnotations join Map where MorseGraphIndex=Map.source;' >> commands.sql

    echo 'explain query plan create table newSig as select ParameterIndex, Map.target as MorseGraphIndex from mergeMe.Signatures join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'create table newSig as select ParameterIndex, Map.target as MorseGraphIndex from mergeMe.Signatures join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'explain query plan insert or ignore into xSignatures select * from newSig;' >> commands.sql
    echo 'insert or ignore into xSignatures select * from newSig;' >> commands.sql
    echo 'drop table newSig;' >> commands.sql

#    echo 'explain query plan insert or ignore into xSignatures select ParameterIndex, Map.target as MorseGraphIndex from mergeMe.Signatures join Map where MorseGraphIndex=Map.source;' >> commands.sql    
#    echo 'insert or ignore into xSignatures select ParameterIndex, Map.target as MorseGraphIndex from mergeMe.Signatures join Map where MorseGraphIndex=Map.source;' >> commands.sql
    
    echo 'drop table Map;' >> commands.sql

    echo 'insert or ignore into xNetwork select * from mergeMe.Network;'
    cat commands.sql | sqlite3 $target
    rm commands.sql
done

# Rename tables
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'alter table xMorseGraphViz rename to MorseGraphViz;' >> commands.sql
echo 'alter table xMorseGraphVertices rename to MorseGraphVertices;' >> commands.sql
echo 'alter table xMorseGraphEdges rename to MorseGraphEdges;' >> commands.sql
echo 'alter table xMorseGraphAnnotations rename to MorseGraphAnnotations;' >> commands.sql
echo 'alter table xSignatures rename to Signatures;' >> commands.sql
echo 'alter table xNetwork rename to Network;' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
