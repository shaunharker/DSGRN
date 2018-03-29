Signatures Tutorial
===================

``Signatures`` is a program which computes databses of Morse graphs for an entire Parameter graph. Here is a demonstration.

First, we use the ``Signatures`` program to create an SQLite database. From the DSGRN folder, type:

.. code-block:: bash

  > mpiexec -np 5 Signatures networks/2D_Example_C.txt ./2D_Example_C.db


Tada! Now we have an SQLite database. This is a small example that computes in seconds even on a laptop. So... looks like we're done here, everybody go home.

Just kidding! We have work to do. Let's examine the output. How can we get at this data and how can we use it? To get the full power here, we need to use a database query language. The most widely known is called SQL, which stands for Structured Query Language, and SQLite uses this. 

A crash course in SQL
---------------------

Ultimately, we will have wonderful web interfaces to examine this database, but for right now, since it is an SQLite database, we can use the SQL query interface. So what follows is a tutorial to SQL, tailored to the needs of using it to query the database of dynamic sigatures computed by DSGRN.

Let's fire up SQLite's interactive interface:

.. code-block:: bash

  sqlite3 2D_Example_C.db


Now we should see the prompt:

.. code-block:: bash

  sqlite>


So far, so good.

Tables
------

A database is comprised of tables; we can ask SQLite about what tables we have:

.. code-block:: sql

  .tables

::

  MorseGraphAnnotations  MorseGraphVertices     Signatures           
  MorseGraphEdges        MorseGraphViz        


The information produced by ``Signatures`` is in these tables. Each table has a number of columns and rows. The columns are named. We can find out about the columns of each table by asking about the database's "schema":

.. code-block:: sql

  .schema

.. code-block:: sql

  CREATE TABLE Signatures (ParameterIndex INTEGER PRIMARY KEY, MorseGraphIndex INTEGER);
  CREATE TABLE MorseGraphViz (MorseGraphIndex INTEGER PRIMARY KEY, Graphviz TEXT);
  CREATE TABLE MorseGraphVertices (MorseGraphIndex INTEGER, Vertex INTEGER);
  CREATE TABLE MorseGraphEdges (MorseGraphIndex INTEGER, Source INTEGER, Target INTEGER);
  CREATE TABLE MorseGraphAnnotations (MorseGraphIndex INTEGER, Vertex INTEGER, Label TEXT);
  CREATE INDEX Signatures2 on Signatures (MorseGraphIndex, ParameterIndex);
  CREATE INDEX MorseGraphAnnotations3 on MorseGraphAnnotations (Label, MorseGraphIndex);
  CREATE INDEX MorseGraphViz2 on MorseGraphViz (Graphviz, MorseGraphIndex);
  CREATE INDEX MorseGraphVertices1 on MorseGraphVertices (MorseGraphIndex, Vertex);
  CREATE INDEX MorseGraphVertices2 on MorseGraphVertices (Vertex, MorseGraphIndex);
  CREATE INDEX MorseGraphEdges1 on MorseGraphEdges (MorseGraphIndex);
  CREATE INDEX MorseGraphAnnotations1 on MorseGraphAnnotations (MorseGraphIndex);


What we see here are the commands that were invoked to create the tables and indices. Indices are data structures formed on tables to speed up different types of queries. Indexing is extremely important; we'll talk about it later. For now, we'll focus on the tables. Consider the ``Signatures`` table. There are two columns: ``ParameterIndex`` and ``MorseGraphIndex``. Both hold integer data. The ``ParameterIndex`` is marked ``INTEGER PRIMARY KEY``, which tells SQLite that it doesn't need to add its customary implicit ``rowid`` column; it can assume the ``ParameterIndex`` data can serve that function equally well (in particular, we will never attempt to insert two rows with the same ``ParameterIndex`` value).


Selecting things
----------------

To learn more about tables we will want to see their contents. In order to get at the contents of a table, we use the ``SELECT`` command. Time for an example. The table ``MorseGraphViz`` contains a list of MorseGraphs in a somewhat human-readable graphviz format, so let's take a look at that one.

We'll type

.. code-block:: sql

  select * from MorseGraphViz;


The ``*`` here says you want to know the values for all the columns. 

::

  0|digraph {
  0[label="FP"];
  1[label="FP ON"];
  }

  1|digraph {
  0[label="FP ON"];
  }

  2|digraph {
  0[label="FP ON"];
  1[label="FP"];
  }


skip a few...

::
  
  18|digraph {
  0[label="FP ON"];
  1[label="FP"];
  2[label="FP ON"];
  3[label="FP"];
  4[label="FP"];
  }

  19|digraph {
  0[label="FP ON"];
  1[label="FP"];
  2[label="FP"];
  3[label="FP ON"];
  4[label="FP"];
  }


skip a few more...

::
  
  27|digraph {
  0[label="FP ON"];
  1[label="FP ON"];
  2[label="FP"];
  3[label="FP"];
  4[label="FP OFF"];
  }


As we can see, the database computation recorded 28 distinct Morse graphs. By way of example, we notice Morse graphs 18 and 19 are isomorphic, so by distinct we mean having a distinct representation rather than not being isomorphic. Later, we will add a feature that attempts to reduce the number of Morse graphs by heuristic approaches to graph canonicalization. For now, we can query about both representations at the same time, which is a nice example anyway. We will look in the table ``Signatures``, which provides a mapping between parameter indices and the Morse graphs that occur:

.. code-block:: sql

  select * from Signatures where MorseGraphIndex=18 or MorseGraphIndex=19;

.. code-block:: sql

  147|18
  207|18
  307|18
  1389|18
  1390|18
  1392|18
  150|19
  155|19
  210|19
  215|19
  310|19
  315|19
  1409|19
  1410|19
  1412|19
  1449|19
  1450|19
  1452|19


Indexing
--------

Let's stop a moment and think about how the database answered that last query. Was it efficient? One approach which would be absolutely terrible is if the database software scanned the ``Signature`` table in entirety and returned all the matching rows. For large databases, this would be ridiculously slow. It is called a *full table scan*. Full table scans are to be prevented whenever possible. The means of preventing full table scans is *indexing*. With indexes, the database maintains a copy of the database sorted by other columns. So having sorted the ``Signatures`` table by the ``MorseGraphIndex`` column, one can pull out the matching rows extremely efficiently (just as one can look up a word in a dictionary efficiently). Looking back at the schema, we see that such an index was in fact constructed by DSGRN's computation:

.. code-block:: sql

  CREATE INDEX Signatures2 on Signatures (MorseGraphIndex, ParameterIndex);


Thus, these sorts of queries should be fast. We can actually ask SQLite how it plans to accomplish a query, and make sure it is using our index to do it right, rather than do a full table scan:

.. code-block:: sql

  explain query plan select * from Signatures where MorseGraphIndex=18 or MorseGraphIndex=19;

.. code-block:: sql

  0|0|0|SEARCH TABLE Signatures USING COVERING INDEX Signatures2 (MorseGraphIndex=?)
  0|0|0|EXECUTE LIST SUBQUERY 1


Seems legit. What happens if it didn't have that index?

.. code-block:: sql

  drop index Signatures2;
  explain query plan select * from Signatures where MorseGraphIndex=18 or MorseGraphIndex=19;

.. code-block:: sql

  0|0|0|SCAN TABLE Signatures


Oh no! The dreaded full table scan! Quick, rebuild the index!

.. code-block:: sql

  CREATE INDEX Signatures2 on Signatures (MorseGraphIndex, ParameterIndex);


Let's try another query. For the network we are considering, there were 4 cohorts of 400 parameters. We might only be interested in a particular cohort (i.e. ordering of thresholds) so we only care about, for instance, parameters 0 through 399. We could make a similar query, but restricted to the first 400 parameters:

.. code-block:: sql

  select * from Signatures where (MorseGraphIndex=18 or MorseGraphIndex=19) and ParameterIndex < 400;

.. code-block:: sql

147|18
207|18
307|18
150|19
155|19
210|19
215|19
310|19
315|19


Here SQLite should again use the index, first quickly determining which rows satisfy ``MorseGraphIndex=18 or MorseGraphIndex=19`` and then among these rows using the sorting of ``ParameterIndex`` to quickly retrieve the rows associated with the ``ParameterIndex < 400`` cohort. This is a more complex usage of the index; see SQLite's nice explanation of https://www.sqlite.org/queryplanner.html[query planning] for more details.

Selecting Individual Columns
----------------------------

Back to the example. Let's say we do not care which parameter was assigned Morse graph 18 and which was assigned 19 since they are, after all, isomorphic. We can instruct to only give the column corresponding the parameter index, dropping the column which distinguishes between 18 and 19:

.. code-block:: sql

  select ParameterIndex from Signatures where (MorseGraphIndex=18 or MorseGraphIndex=19) and ParameterIndex < 400;

.. code-block:: sql

  147
  207
  307
  150
  155
  210
  215
  310
  315


We could also ask for the columns out of order, or the same column more than once. For example:

.. code-block:: sql

  select ParameterIndex,ParameterIndex,MorseGraphIndex,ParameterIndex from Signatures where (MorseGraphIndex=18 or MorseGraphIndex=19) and ParameterIndex < 400;


.. code-block:: sql

  147|147|18|147
  207|207|18|207
  307|307|18|307
  150|150|19|150
  155|155|19|155
  210|210|19|210
  215|215|19|215
  310|310|19|310
  315|315|19|315



Group concatenation
-------------------

We may prefer a comma separated list rather than a long column, so we can use the ``GROUP_CONCAT`` feature:

.. code-block:: sql

  select GROUP_CONCAT(ParameterIndex) from Signatures where MorseGraphIndex=18 or MorseGraphIndex=19 and ParameterIndex < 400;

.. code-block:: sql

  147,207,307,1389,1390,1392,150,155,210,215,310,315


If this seems sort of strange, good. It's an example of an *aggregate function*, and we'll talk about those more later.

Joining tables
--------------

An important tool to use database structures is the ``join``. This lets us combine two tables based on matches in a certain way. Let's consider an example. Suppose we want to find all parameters where the Morse graph has at least 4 vertices. Since vertices in a Morse graph are indexed contiguously beginning at zero, this is the collection of parameters for which the Morse graph contains the vertex ``3``. This gives us a hackish trick for quickly selecting them by querying the ``MorseGraphVertices`` table for ``Vertex=3``. The select will be fast, since we have the following index available:

.. code-block:: sql

  CREATE INDEX MorseGraphVertices2 on MorseGraphVertices (Vertex, MorseGraphIndex);


SQLite will use this index to quickly answer the following query:

.. code-block:: sql

  select GROUP_CONCAT(MorseGraphIndex) from MorseGraphVertices where Vertex=3;

.. code-block:: sql

  8,10,13,16,18,19,21,22,24,25,26,27


But wait; these aren't the parameters with Morse graphs having at least 4 vertices; these are the Morse graphs themselves. We need to turn around and look up parameters associated with these Morse graphs. Here is how *NOT* to do it:

.. code-block:: sql

  select ParameterIndex from Signatures where MorseGraphIndex=8 or MorseGraphIndex=10 or ...


Nope! Ack! Don't do that. Instead, try this:

.. code-block:: sql

  create temp table GotAtLeastFour as select MorseGraphIndex from MorseGraphVertices where Vertex=3;


Now ``GotAtLeastFour`` is the table of results we were just considering. We told SQLite it was temporary, so it knows this isn't going to be a permanent citizen of our database. We can use it with the ``Signatures`` table as follows:

.. code-block:: sql

  select GROUP_CONCAT(ParameterIndex) from Signatures natural join GotAtLeastFour;

.. code-block:: sql

  693,713,1007,1047,1269,1270,1271,1272,1273,1391,1393,1493,990,995,1010,1015,1050,1055,122,127,182,187,282,287,288,688,821,822,861,862,866,867,868,922,927,982,986,988,1082,1087,1088,1266,1267,1268,1386,1387,1388,1488,142,202,222,227,302,322,327,328,728,732,753,1289,1290,1291,1292,1293,1411,1413,1451,1453,1513,1553,147,207,307,1389,1390,1392,150,155,210,215,310,315,1409,1410,1412,1449,1450,1452,308,312,317,692,708,717,752,1492,1512,1552,144,204,224,230,235,304,324,330,332,335,337,737,549,550,552,609,610,612,709,710,712,733,738,773,778,987


The ``natural join`` was the `magic mustard`__ here. The ``natural`` keyword is shorthand to tell it to perform a join on the columns with the same name. A ``join`` is a procedure whereby two tables are combined together into a single table on the basis of matching entries. 

__  http://en.wikipedia.org/wiki/Join_(SQL)#Natural_join_.28.E2.8B.88.29

We can get rid of that temporary table now:

.. code-block:: sql

  drop table GotAtLeastFour;


Good riddance. In fact, we never needed that pesky tempory table in the first place:

.. code-block:: sql

  select GROUP_CONCAT(ParameterIndex) from Signatures natural join (select MorseGraphIndex from MorseGraphVertices where Vertex=3);

.. code-block:: sql

  693,713,1007,1047,1269,1270,1271,1272,1273,1391,1393,1493,990,995,1010,1015,1050,1055,122,127,182,187,282,287,288,688,821,822,861,862,866,867,868,922,927,982,986,988,1082,1087,1088,1266,1267,1268,1386,1387,1388,1488,142,202,222,227,302,322,327,328,728,732,753,1289,1290,1291,1292,1293,1411,1413,1451,1453,1513,1553,147,207,307,1389,1390,1392,150,155,210,215,310,315,1409,1410,1412,1449,1450,1452,308,312,317,692,708,717,752,1492,1512,1552,144,204,224,230,235,304,324,330,332,335,337,737,549,550,552,609,610,612,709,710,712,733,738,773,778,987


We should stay in the habit of always making sure the queries we run are efficient:
.. code-block:: sql

  explain query plan select GROUP_CONCAT(ParameterIndex) from Signatures natural join (select MorseGraphIndex from MorseGraphVertices where Vertex=3);

.. code-block:: sql

  0|0|1|SEARCH TABLE MorseGraphVertices USING COVERING INDEX MorseGraphVertices2 (Vertex=?)
  0|1|0|SEARCH TABLE Signatures USING COVERING INDEX Signatures2 (MorseGraphIndex=?)

We see its plan involves using the proper indices, so it seems right.

Counting things
---------------

The last example was pretty fancy, so let's do something a little more basic: counting.

There is a command ``COUNT`` which lets you count things:

.. code-block:: sql

  select COUNT(*) from Signatures;

.. code-block:: sql

  1600


Those are our 1600 parameters. There is also a ``distinct`` keyword, which lets us select only when a new item is found. Here is an example of its use:

.. code-block:: sql

  select COUNT(distinct MorseGraphIndex) from Signatures;

.. code-block:: sql

  28


Looks great, but _beware!_ You might assume these are fast operations, the former taking :math:`O(1)` time and the latter taking (for an indexed table) :math:`O(n \log N)` time, where :math:`n` is the count returned and :math:`N` is the number of rows. However, SQLite has apparently `made a deliberate design decision`__ which forces a full table scan whenever ``COUNT`` is used. We can actually speed up the latter:

__ http://www.mail-archive.com/sqlite-users@sqlite.org/msg10279.html

.. code-block:: sql

  select COUNT(*) from (select distinct MorseGraphIndex from Signatures);

This still runs a full table scan with ``COUNT``, but it does it on the small table returned by the interior ``SELECT``. Meanwhile, the ``select distinct MorseGraphIndex from Signatures`` *does the right thing*:

.. code-block:: sql

  explain query plan select distinct MorseGraphIndex from Signatures;

::

  0|0|0|SCAN TABLE Signatures USING COVERING INDEX Signatures2


Well, hopefully does the right thing. Technically, the asymptotically best solution is based on skipping from key to key with repeated binary searches, not scanning the entire indexed table. http://sqlite.1065341.n5.nabble.com/Index-performance-td72375.html[Word on the street] is that this so-called skip-scan algorithm is only considered when there are more than 50 rows for each distinct value. In the current example, there are around 57 (we'll see this later in an example), but the database is very small, so it might not bother. Definitely worth checking out as things scale up!

Aggregate functions
-------------------

The two expressions we have seen, ``GROUP_CONCAT`` and ``COUNT`` are examples of what are known as *aggregate functions*. The idea of an aggregate function is that it takes a column and compresses it into a single item. If ``A`` is an aggregate function, then there is some seed value (for ``GROUP_CONCAT`` it is the empty string, for ``COUNT`` it is 0) and upon processing a new item it updates the value according to what it sees. For ``GROUP_CONCAT`` it affixes the next item with a comma delimiter. For ``COUNT`` it adds one. 

In fact, we can combine aggregate functions together in queries in comma-concatenated lists. When we do this, we can regard vanilla column entries as aggregate functions as well: ``MorseGraphIndex`` can be understood as the aggregate function that reads a row and updates its value to be the entry in the ``MorseGraphIndex`` column. Here is an example:

.. code-block:: sql

  select MorseGraphIndex,COUNT(Vertex) from MorseGraphVertices;

Which yields:
.. code-block:: sql

  27|89


The 89 makes sense; this is the total number of vertices in all the Morse graphs combined. But what is the 27? It isn't the number of Morse graphs; there are 28 of them. The answer is that it is simply the last value of the ``MorseGraphIndex`` field encountered while processing the rows with the aggregate function.

For a very ridiculous example (purely to illustrate the behavior of aggregate functions):

.. code-block:: sql

  select GROUP_CONCAT(MorseGraphIndex),GROUP_CONCAT(Vertex) from MorseGraphVertices;

.. code-block:: sql

  0,0,1,2,2,3,3,3,4,4,4,5,5,6,7,7,7,8,8,8,8,9,9,9,10,10,10,10,11,11,12,12,12,13,13,13,13,14,14,15,16,16,16,16,17,17,17,18,18,18,18,18,19,19,19,19,19,20,20,20,21,21,21,21,22,22,22,22,23,23,23,24,24,24,24,25,25,25,25,25,26,26,26,26,27,27,27,27,27|0,1,0,0,1,0,1,2,0,1,2,0,1,0,0,1,2,0,1,2,3,0,1,2,0,1,2,3,0,1,0,1,2,0,1,2,3,0,1,0,0,1,2,3,0,1,2,0,1,2,3,4,0,1,2,3,4,0,1,2,0,1,2,3,0,1,2,3,0,1,2,0,1,2,3,0,1,2,3,4,0,1,2,3,0,1,2,3,4


This has made comma-concatenated lists out of every ``MorseGraphIndex`` field and every ``Vertex`` field, and made a row with two columns out of them.


Grouping things
---------------

At this point one might get the idea that aggregate functions are a one-trick pony, good for counting the number of rows or concatenating entries from a single column but yielding nonsense in all other situations. This is because we have not yet covered the ``GROUP BY`` syntax, which makes aggregate functions far more useful.

The ``GROUP BY`` command tells a ``SELECT`` statement to break the rows into groups based on chosen columns. For example, ``GROUP BY MorseGraphIndex`` would tell SQLite you wanted to look at the table in several groups, each group having some given value in the ``MorseGraphIndex`` field. It automatically assumes you mean to use aggregate functions. The aggregate functions don't process the entire table and produce a single row, but rather process each group and produce a table with rows corresponding to each group. This is quite useful.

We'll illustrate this by example. Let's figure out which Morse graphs have more than 50 parameters corresponding to them. We tackle this by first creating a table with rows reporting the number of parameters associated to each Morse graph. Then we select those rows for which there are more than 50 parameters.

.. code-block:: sql

  select MorseGraphIndex,COUNT(*) as Frequency from Signatures group by MorseGraphIndex order by Frequency desc;


::

  6|418
  14|266
  2|204
  1|126
  17|74
  0|70
  9|69
  11|64
  5|50
  7|47
  12|33
  13|30
  3|23
  16|22
  15|16
  8|12
  19|12
  22|12
  21|10
  24|8
  20|7
  4|6
  10|6
  18|6
  26|4
  23|3
  25|1
  27|1


Wonderful! But how did it work? There are several new pieces of syntax. First, we see the ``count(*) as Frequency``. This tells SQLite that the thing it is computing with the aggregate function ``count(*)`` is something that you want to refer to by the name ``Frequency``. Later, we have the phrase ``order by Frequency desc`` which tells SQLite you want the final table to be ordered by the ``Frequency`` column in descending order. Finally, saving the best for last, we have the ``group by MorseGraphIndex``. The ``GROUP BY`` feature tells SQLite that we want to break the table into groups based on the value of the ``MorseGraphIndex`` column. It then "evaluates" the aggregate function ``MorseGraphIndex,COUNT(*)`` over each group, and the output for each group is the row consisting of the ``MorseGraphIndex`` for the group and the number of rows in that group. Which is what we wanted! So along with ``GROUP BY``, aggregate functions become powerful tools.

As a quick aside, we can now calculate the average number of parameters associated to a Morse graph by using the ``AVG`` aggregate function:
.. code-block:: sql

  select AVG(Frequency) from MorseGraphClasses;

.. code-block:: sql

  57.1428571428571


Anyhow, we are now ready to give the list of all Morse graphs have more than 50 parameters corresponding to them:

.. code-block:: sql

  select GROUP_CONCAT(MorseGraphIndex) from (select MorseGraphIndex,count(*) as Frequency from Signatures group by MorseGraphIndex order by Frequency desc) where Frequency > 50;

::

  6,14,2,1,17,0,9,11


More grouping
-------------

Let's do an even more glamorous example of grouping. Let's create a table with three columns. The first column will be ``MorseGraphIndex``, the second column will be ``Frequency``, and the third column will be ``ParameterIndices``. Also, let's make sure the table is sorted in descending order on the ``Frequency`` column. 

.. code-block:: sql

  create temp table MorseGraphClasses as select MorseGraphIndex,COUNT(*) as Frequency,GROUP_CONCAT(ParameterIndex) from Signatures group by MorseGraphIndex order by Frequency desc;


Is this table what we want? Instead of ``SELECT * FROM MorseGraphClasses``, let's instead go with

.. code-block:: sql

  select * from MorseGraphClasses where Frequency < 10

.. code-block:: sql

  24|8|549,550,552,609,610,612,709,710
  20|7|777,1429,1430,1432,1469,1470,1472
  4|6|372,377,1030,1035,1070,1075
  10|6|990,995,1010,1015,1050,1055
  18|6|147,207,307,1389,1390,1392
  26|4|733,738,773,778
  23|3|232,237,637
  25|1|712
  27|1|987


Great! Just what we wanted. But is it doing it efficiently? You'd think because we told it ``order by Frequency desc`` it would know that it could use binary search to do this query. Nope! 

.. code-block:: sql

  explain query plan select * from MorseGraphClasses where Frequency < 10; 

.. code-block:: sql

  0|0|0|SCAN TABLE MorseGraphClasses


Ah, the dreaded full table scan. Probably the rationale is that you could later insert rows which break this property, so it cannot make this assumption. To prevent the full table scan we could index our new table:

.. code-block:: sql

  create index MorseGraphClasses2 on MorseGraphClasses (Frequency);


Now the database software has twigged to the better plan:

.. code-block:: sql

  explain query plan select * from MorseGraphClasses where Frequency < 10; 

.. code-block:: sql

  0|0|0|SEARCH TABLE MorseGraphClasses USING INDEX MorseGraphClasses2 (Frequency<?)


Even more grouping
------------------

Let's revisit the example with finding Morse graphs with a certain number of vertices. Before, we used a hackish trick that relied on the fact a Morse graph had a vertex with index :math:`k` if and only if it had at least :math:`k+1` vertices. Using grouping, we don't need to rely on such tricks.

We'll make a table which lists each Morse graph according to the number of vertices it has in descending order.

.. code-block:: sql

  create temp table MorseGraphVertexCount as select MorseGraphIndex,COUNT(*) as VertexCount from MorseGraphVertices group by MorseGraphIndex order by VertexCount desc;


Now let's select the ones with more than 3 vertices:

.. code-block:: sql

  select GROUP_CONCAT(MorseGraphIndex) from MorseGraphVertexCount where VertexCount > 3;

.. code-block:: sql

  18,19,25,27,8,10,13,16,21,22,24,26


This is the same collection of Morse graphs we found earlier, albeit in a different order.

Combinations of Labels
----------------------

Our Morse graphs come equipped with annotations. These are visible in the graphviz records, but they are also available in the ``MorseGraphAnnotations`` table. This lets us use the querying capabilities. We might be interested in querying for various logical combinations of annotations applied to a Morse graph, and then learning which parameters are associated.

Let's say we are interested in finding all parameters which have a Morse graph that has an annotation "FP OFF", an annotation "FC", but do not have the annotation "FP ON". We can accomplish this as follows:

.. code-block:: sql

  create temp table HasFPOFF as select MorseGraphIndex from MorseGraphAnnotations where Label="FP OFF";
  create temp table HasFC as select MorseGraphIndex from MorseGraphAnnotations where Label="FC";
  create temp table HasFPON as select MorseGraphIndex from MorseGraphAnnotations where Label="FP ON";
  select GROUP_CONCAT(ParameterIndex) from Signatures natural join (select * from HasFPOFF union select * from HasFC except select * from HasFPON);

.. code-block:: sql

  1,2,6,7,8,21,22,26,27,28,61,62,66,67,68,120,180,280,406,407,408,426,427,428,466,467,468,520,521,522,580,581,582,680,681,682,801,802,806,807,808,820,860,920,980,1080,1206,1207,1208,1220,1221,1222,1260,1261,1262,1320,1321,1322,1380,1381,1382,1480,1481,1482,121,126,128,181,186,188,281,286,526,527,528,586,587,588,686,687,826,827,828,921,926,928,981,1081,1086,1226,1227,1228,1326,1327,1328,1486,1487,0,20,60,400,401,402,420,421,422,460,461,462,800,1200,1201,1202


We check the query plans to make sure this is happening efficiently:
.. code-block:: sql

  explain query plan select MorseGraphIndex from MorseGraphAnnotations where Label="FP ON";

.. code-block:: sql

  0|0|0|SEARCH TABLE MorseGraphAnnotations USING COVERING INDEX MorseGraphAnnotations3 (Label=?)

.. code-block:: sql

  explain query plan select GROUP_CONCAT(ParameterIndex) from Signatures natural join (select * from HasFPOFF union select * from HasFC except select * from HasFPON);

.. code-block:: sql

  3|0|0|SCAN TABLE HasFPOFF
  4|0|0|SCAN TABLE HasFC
  2|0|0|COMPOUND SUBQUERIES 3 AND 4 USING TEMP B-TREE (UNION)
  5|0|0|SCAN TABLE HasFPON
  1|0|0|COMPOUND SUBQUERIES 2 AND 5 USING TEMP B-TREE (EXCEPT)
  0|0|1|SCAN SUBQUERY 1
  0|1|0|SEARCH TABLE Signatures USING AUTOMATIC COVERING INDEX (MorseGraphIndex=?)


Looks good.

A Lesson in Query Planning
--------------------------

It is important to make sure that the database is being efficient in answering queries. Just because there is an efficient way to do something does not mean that SQLite will translate our intent into an efficient search. We earlier saw an example where we needed to create an index to get SQLite to do the right thing. We noticed with the ``COUNT`` function that SQLite forced full table scans. Here we give another example where SQLite actually has the correct indices, but doesn't do the most efficient thing. To this end we try using the ``distinct`` keyword with ``GROUP_CONCAT``:

.. code-block:: sql

  select GROUP_CONCAT(distinct MorseGraphIndex) from Signatures; 

.. code-block:: sql

  15,11,6,2,0,1,14,7,9,12,13,16,22,5,17,18,19,3,23,21,4,24,8,25,26,20,27,10


This result is a little bit disturbing: it is simply the Morse graph indices occurring in the ``Signatures`` table in the order which they appear. This implies that SQLite chose a full table scan to satisfy this query! Let's check:

.. code-block:: sql

  explain query plan select GROUP_CONCAT(distinct MorseGraphIndex) from Signatures;


.. code-block:: sql

  0|0|0|SCAN TABLE Signatures


A full table scan! Terrible! The culprit is the ``GROUP_CONCAT`` and not ``distinct``, as the following shows:

.. code-block:: sql

  select distinct MorseGraphIndex from Signatures;

which returns

::

  0
  1
  2



This indicates the query used the index efficiently:

.. code-block:: sql

  explain query plan select distinct MorseGraphIndex from Signatures;

.. code-block:: sql

  0|0|0|SCAN TABLE Signatures USING COVERING INDEX Signatures2


Is this a bug in SQLite? Well, we could argue that SQLite believed that we were very much interested in the order the distinct ``MorseGraphIndex`` elements occurred, row by row, in the ``Signatures`` table. Thus a full table scan was required to ascertain this ordering.

So if I wanted the comma-separated list without a slow full-table scan, a solution would be

.. code-block:: sql

  select GROUP_CONCAT(MorseGraphIndex) from (select distinct MorseGraphIndex from Signatures);

.. code-block:: sql

  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27

.. code-block:: sql

  explain query plan select GROUP_CONCAT(MorseGraphIndex) from (select distinct MorseGraphIndex from Signatures);

.. code-block:: sql

  1|0|0|SCAN TABLE Signatures USING COVERING INDEX Signatures2
  0|0|0|SCAN SUBQUERY 1


The moral of the story here is to check the query plans, especially when you are using aggregate functions. The difference between :math:`O(N)` time full table scan versus a :math:`O(n \log N)` time binary search is dramatic for usual choices of :math:`n` and :math:`N`. 
