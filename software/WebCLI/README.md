# Web Command Line Interface

This uses Emscripten.
In order to get it to work, we need to compile the dependencies boost and sqlite3 using emscripten. Since boost is huge and has a strange build system which doesn't play well with emscripten, this isn't really practical. So instead we compile only the serialization part we require.

Meanwhile the single-header/single-source file distribution of sqlite3 in C code makes it absolutely painless to handle.

Currently the ./install.sh script works on a Mac OS X with boost and emscripten installed with brew. To work elsewhere may require modifications.


