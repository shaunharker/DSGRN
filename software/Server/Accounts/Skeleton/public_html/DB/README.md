# DSGRN Web Database Interface

Notes.

Databases are kept in the "data" folder.

In order to function, two things must be done.
(1) They must be added to the registry.db database.
(2) They must have appropriate post-processing

To accomplish step (1), run the
./buildRegistry.sh
script.

To accomplish step (2), run the 
./postprocess.sh 
script. This create MorseGraphZoo tables in the databases.

In the event that the post-processing changes, there is a script
./droptables.sh
provided.

Also, viz.js (https://github.com/mdaines/viz.js/releases/download/0.0.3/viz.js) should be put in the js folder.

Acknowledgements:

This code uses Graphviz, in particular the emscripten port of
https://github.com/mdaines

In addition the databases are handled by SQLite3.
