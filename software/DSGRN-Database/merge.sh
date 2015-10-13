#!/bin/bash
# Merge script
# ./merge.sh folder target
# Merges all DSGRN databases in folder 
# into a single database named target

# Initialize (erase target if already exists)
folder=$1
target=$2
rm -rf $target

# Create List of all Morse Graphs (in graphviz form)
sqlite3 $target 'create table MGList (Graphviz TEXT, unique(Graphviz));'
for db in `ls $folder`; do
    echo $db
    echo "attach '$folder/$db' as mergeMe;" > commands.sql
    echo "insert or ignore into MGList select Graphviz from mergeMe.MorseGraphViz;" >> commands.sql
    cat commands.sql | sqlite3 $target
    rm commands.sql
done

# Enumerate Morse graphs (i.e. give MorseGraphIndex field)
# and initialize database
echo 'create table MorseGraphViz (MorseGraphIndex INTEGER PRIMARY KEY, Graphviz TEXT);' > commands.sql
echo 'insert or ignore into MorseGraphViz select rowid as MorseGraphIndex,Graphviz from MGList;' >> commands.sql
echo 'drop table MGList;' >> commands.sql
echo 'create table MorseGraphVertices (MorseGraphIndex INTEGER, Vertex INTEGER, unique(MorseGraphIndex,Vertex));' >> commands.sql
echo 'create table MorseGraphEdges (MorseGraphIndex INTEGER, Source INTEGER, Target INTEGER, unique(MorseGraphIndex,Source,Target));' >> commands.sql
echo 'create table MorseGraphAnnotations (MorseGraphIndex INTEGER, Vertex INTEGER, Label TEXT, unique(MorseGraphIndex,Vertex,Label));' >> commands.sql
echo 'create table Signatures (ParameterIndex INTEGER PRIMARY KEY, MorseGraphIndex INTEGER);' >> commands.sql
echo "create index if not exists Signatures2 on Signatures (MorseGraphIndex, ParameterIndex);" >> commands.sql
echo "create index if not exists MorseGraphAnnotations3 on MorseGraphAnnotations (Label, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphViz2 on MorseGraphViz (Graphviz, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphVertices1 on MorseGraphVertices (MorseGraphIndex, Vertex);" >> commands.sql
echo "create index if not exists MorseGraphVertices2 on MorseGraphVertices (Vertex, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphEdges1 on MorseGraphEdges (MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphAnnotations1 on MorseGraphAnnotations (MorseGraphIndex);" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql

# Merge databases
for db in `ls $folder`; do
    echo $db
    echo "attach '$folder/$db' as mergeMe;" > commands.sql
    echo 'create table Map as select S.MorseGraphIndex as source,T.MorseGraphIndex as target from (mergeMe.MorseGraphViz as S join MorseGraphViz as T on S.Graphviz=T.Graphviz);' >> commands.sql
    echo 'insert or ignore into MorseGraphVertices select Map.target as MorseGraphIndex, Vertex from mergeMe.MorseGraphVertices join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'insert or ignore into MorseGraphEdges select Map.target as MorseGraphIndex, mergeMe.MorseGraphEdges.Source, mergeMe.MorseGraphEdges.Target from mergeMe.MorseGraphEdges join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'insert or ignore into MorseGraphAnnotations select Map.target as MorseGraphIndex, Vertex, Label from mergeMe.MorseGraphAnnotations join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'insert or ignore into Signatures select ParameterIndex, Map.target as MorseGraphIndex from mergeMe.Signatures join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'explain query plan insert or ignore into MorseGraphVertices select Map.target as MorseGraphIndex, Vertex from mergeMe.MorseGraphVertices join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'explain query plan insert or ignore into MorseGraphEdges select Map.target as MorseGraphIndex, mergeMe.MorseGraphEdges.Source, mergeMe.MorseGraphEdges.Target from mergeMe.MorseGraphEdges join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'explain query plan insert or ignore into MorseGraphAnnotations select Map.target as MorseGraphIndex, Vertex, Label from mergeMe.MorseGraphAnnotations join Map where MorseGraphIndex=Map.source;' >> commands.sql
    echo 'explain query plan insert or ignore into Signatures select ParameterIndex, Map.target as MorseGraphIndex from mergeMe.Signatures join Map where MorseGraphIndex=Map.source;' >> commands.sql

    echo 'drop table Map;' >> commands.sql
    cat commands.sql | sqlite3 $target
    rm commands.sql
done
