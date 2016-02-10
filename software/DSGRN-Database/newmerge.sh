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
#echo 'Indexing database shards...'
#for db in `ls $folder`; do
#    echo $db
#    echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
#    echo "drop index MorseGraphVizAsc;" >> commands.sql
#    echo "drop index MorseGraphVertices1;" >> commands.sql
#    echo "drop index MorseGraphEdges1;" >> commands.sql
#    echo "drop index MorseGraphAnnotations1;" >> commands.sql
#    echo "create index if not exists MorseGraphVizAsc on MorseGraphViz (Graphviz ASC, MorseGraphIndex);" >> commands.sql
#    echo "create index if not exists MorseGraphVertices1 on MorseGraphVertices (MorseGraphIndex, Vertex);" >> commands.sql
#    echo "create index if not exists MorseGraphEdges1 on MorseGraphEdges (MorseGraphIndex, Source, Target);" >> commands.sql
#    echo "create index if not exists MorseGraphAnnotations1 on MorseGraphAnnotations (MorseGraphIndex, Vertex, Label);" >> commands.sql
#    cat commands.sql | sqlite3 $folder/$db
#    rm commands.sql
#done

# Create List of all Morse Graphs (in graphviz form)
echo 'Scanning database shards...'
sqlite3 $target 'create table BigMorseGraphviz (Graphviz TEXT, ShardIndex INTEGER, Shard TEXT);'
for db in `ls $folder`; do
    echo $db
    echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
    echo "attach '$folder/$db' as mergeMe;" >> commands.sql
    echo "insert into BigMorseGraphviz select Graphviz, MorseGraphIndex, '$db' from mergeMe.MorseGraphViz;" >> commands.sql
    cat commands.sql | sqlite3 $target
    rm commands.sql
    echo `date`
done


echo "Indexing amalgamated Morse graph information..."
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql   
echo "create index BigMorseGraphviz1 on BigMorseGraphviz (Graphviz, ShardIndex, Shard);" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo "Creating Master MorseGraph table..."
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql   
echo "create temp table MGList as select distinct Graphviz from BigMorseGraphviz;" >> commands.sql
echo "create table xMorseGraphviz (MorseGraphIndex INTEGER PRIMARY KEY, Graphviz TEXT);" >> commands.sql
echo "explain query plan insert into xMorseGraphviz select rowid as MorseGraphIndex, Graphviz from MGList;" >> commands.sql
echo "insert into xMorseGraphviz select rowid as MorseGraphIndex, Graphviz from MGList;" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo "Indexing Master MorseGraph table..."
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql   
echo "create index MorseGraphViz2 on xMorseGraphViz (Graphviz, MorseGraphIndex);" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo "Creating Mapping table..."
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql   
echo "create table BigMap (ShardIndex INTEGER, Shard TEXT, GlobalIndex INTEGER);" >> commands.sql
echo "explain query plan insert into BigMap select ShardIndex, Shard, MorseGraphIndex from xMorseGraphviz natural join BigMorseGraphviz;" >> commands.sql
echo "insert into BigMap select ShardIndex, Shard, MorseGraphIndex from xMorseGraphviz natural join BigMorseGraphviz;" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo "Indexing Mapping table..."
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql   
echo "create index BigMap2 on BigMap (Shard, ShardIndex, GlobalIndex);" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo "Dropping amalgamated table..."
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql   
echo "drop table BigMorseGraphviz;" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

# Initialize merged database
echo 'Initializing database'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'create table xMorseGraphVertices (MorseGraphIndex INTEGER, Vertex INTEGER);' >> commands.sql
echo 'create table xMorseGraphEdges (MorseGraphIndex INTEGER, Source INTEGER, Target INTEGER);' >> commands.sql
echo 'create table xMorseGraphAnnotations (MorseGraphIndex INTEGER, Vertex INTEGER, Label TEXT);' >> commands.sql
echo 'create table xSignatures (ParameterIndex INTEGER PRIMARY KEY, MorseGraphIndex INTEGER);' >> commands.sql
echo 'create table xNetwork ( Name TEXT PRIMARY KEY, Dimension INTEGER, Specification TEXT, Graphviz TEXT);' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

# Merge databases
echo 'Merging database shards...'
for db in `ls $folder`; do
    echo $db
    echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
    echo "attach '$folder/$db' as mergeMe;" >> commands.sql
    
    echo "explain query plan insert into xMorseGraphVertices select GlobalIndex as MorseGraphIndex, Vertex from mergeMe.MorseGraphVertices join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql
    echo "insert into xMorseGraphVertices select GlobalIndex as MorseGraphIndex, Vertex from mergeMe.MorseGraphVertices join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql
    
    echo "explain query plan insert into xMorseGraphEdges select GlobalIndex as MorseGraphIndex, mergeMe.MorseGraphEdges.Source, mergeMe.MorseGraphEdges.Target from mergeMe.MorseGraphEdges join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql
    echo "insert into xMorseGraphEdges select GlobalIndex as MorseGraphIndex, mergeMe.MorseGraphEdges.Source, mergeMe.MorseGraphEdges.Target from mergeMe.MorseGraphEdges join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql

    echo "explain query plan insert into xMorseGraphAnnotations select GlobalIndex as MorseGraphIndex, Vertex, Label from mergeMe.MorseGraphAnnotations join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql    
    echo "insert into xMorseGraphAnnotations select GlobalIndex as MorseGraphIndex, Vertex, Label from mergeMe.MorseGraphAnnotations join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql

    echo "explain query plan insert or ignore into xSignatures select ParameterIndex, GlobalIndex as MorseGraphIndex from mergeMe.Signatures join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql    
    echo "insert or ignore into xSignatures select ParameterIndex, GlobalIndex as MorseGraphIndex from mergeMe.Signatures join BigMap where MorseGraphIndex=ShardIndex and Shard='$db';" >> commands.sql

    echo "insert or ignore into xNetwork select '$1',Dimension,Specification,Graphviz from mergeMe.Network;" >> commands.sql
    cat commands.sql | sqlite3 $target
    rm commands.sql
    echo `date`
done

# Rename tables
echo 'Dropping mapping table...'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'drop table BigMap;' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo 'Finalizing MorseGraphViz...'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'alter table xMorseGraphViz rename to MorseGraphViz;' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo 'Finalizing MorseGraphVertices...'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'create index xMorseGraphVertices1 on xMorseGraphVertices (MorseGraphIndex, Vertex);' >> commands.sql
echo 'create table MorseGraphVertices as select distinct * from xMorseGraphVertices;' >> commands.sql
echo 'drop table xMorseGraphVertices;' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo 'Finalizing MorseGraphEdges...'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'create index xMorseGraphEdges1 on xMorseGraphEdges (MorseGraphIndex, Source, Target);' >> commands.sql
echo 'create table MorseGraphEdges as select distinct * from xMorseGraphEdges;' >> commands.sql
echo 'drop table xMorseGraphEdges;' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo 'Finalizing MorseGraphAnnotations...'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'create index xMorseGraphAnnotations1 on xMorseGraphAnnotations (MorseGraphIndex, Vertex, Label);' >> commands.sql
echo 'create table MorseGraphAnnotations as select distinct * from xMorseGraphAnnotations;' >> commands.sql
echo 'drop table xMorseGraphAnnotations;' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

echo 'Finalizing Signatures...'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo 'alter table xSignatures rename to Signatures;' >> commands.sql
echo 'alter table xNetwork rename to Network;' >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
echo `date`

# Form indices
echo 'Forming indices'
echo "PRAGMA temp_store_directory = '$(pwd)';" > commands.sql    
echo "create index if not exists Signatures2 on Signatures (MorseGraphIndex, ParameterIndex);" >> commands.sql
echo "create index if not exists MorseGraphAnnotations3 on MorseGraphAnnotations (Label, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphViz2 on MorseGraphViz (Graphviz, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphVertices1 on MorseGraphVertices (MorseGraphIndex, Vertex);" >> commands.sql
echo "create index if not exists MorseGraphVertices2 on MorseGraphVertices (Vertex, MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphEdges1 on MorseGraphEdges (MorseGraphIndex);" >> commands.sql
echo "create index if not exists MorseGraphAnnotations1 on MorseGraphAnnotations (MorseGraphIndex);" >> commands.sql
cat commands.sql | sqlite3 $target
rm commands.sql
