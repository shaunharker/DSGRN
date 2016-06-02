# Python

## Overview
Here we have implemented python wrappers to allow DSGRN types and algorithms to be used in the Python programming language.

## Development Status

This remains in development. A web deployment is available in the `web` subfolder.

## Dependencies

This project requires that DSGRN be built, and also requires boost python to run. Note that boost python might not be built even if boost is built on Mac OS X; to remedy this type

### Boost
```bash
brew install boost
brew install boost-python
```


### Anaconda
To use the iPython Notebook feature we recommend a python distribution such as Anaconda. This has a simple GUI installer on Mac OS X. However, there appears to be a problem on Mac OS X with the libpython2.7.dylib file produced which causes a linking issue. To fix it, type

```bash
install_name_tool -id @rpath/libpython2.7.dylib anaconda/lib/libpython2.7.dylib
```

See http://stackoverflow.com/questions/23771608/trouble-installing-galsim-on-osx-with-anaconda for details.

### Graphviz

Also required is graphviz; on Mac OS X this may be obtained with: 

```bash
brew install graphviz --with-app --with-bindings
pip install graphviz
```


## Installation

The installer program will attempt to install a module in the site-packages directory of the detected python installation.

To install

```bash
./install.sh
```

If it complains about administrator rights, you can give it an alternative installation prefix:

```bash
./install.sh --prefix=/your/installation/prefix
```

Here you should use whichever installation prefix you used to install DSGRN. Note that if the installation prefix is not given it defaults to `/usr/local`.

The installer will create a python module named `DSGRN` and store a copy of it in
```bash
/your/installation/prefix/share/DSGRN/modules/DSGRN
```

In order for this module to be usable by Python you have two options:

1. Install the module into your python distribution, or

2. Alter your `PYTHONPATH` system variable

For approach (1), type the following:
```bash
cd modules
python setup.py install
```

However, if you are using a python distribution you do not have rights to, this approach for fail. In this case, the second approach is necessary. 

For approach (2), edit your `~/.bashrc` script and add the line:

```bash
export PYTHONPATH=/usr/local/share/DSGRN/modules:$PYTHONPATH
```

NOTE: If you used the `--prefix` for the installation, use your installation prefix in place of `/usr/local`.


## Usage

Type `python` to start the python interpreter, and then:

```python
import DSGRN
network = DSGRN.Network()
network.assign('X : (X)(Y+~Z)\nY : X\nZ : (X)(~Y)\n')
print network.size()
print network.inputs(0)
print network.name(0)
print network.index("X")
print network.logic(0)
print network.graphviz()
print network.interaction(2,0)
# etc...
```

For iPython notebook documentation, type

```bash
jupyter notebook
```
and view the `DSGRN_Python_GettingStarted.ipynb` notebook in the `doc` folder. Note this can be viewed on Github's website as well.

