#!/bin/bash

rm -f registry.db
echo 'create table Networks ( Filename TEXT, Name Text, Dimension INTEGER, Specification TEXT, Graphviz TEXT);' > registry.sql
for file in `ls data/*.db`; do
    echo "attach '$file' as db;" >> registry.sql
    echo "insert into Networks select '$file', * from db.Network;" >> registry.sql
    echo "detach db;" >> registry.sql
done

cat registry.sql | sqlite3 registry.db > /dev/null
rm registry.sql
