# Python

## Overview
Here we have implemented python wrappers to allow DSGRN types and algorithms to be used in the Python programming language.

## Development Status

This is currently a work in progress and only contains a test program. The main accomplishment so far is an installer.

It has been tested on Mac OS X and is expected to work on Linux. It is not expected to work on Windows systems.

## Dependencies

This project requires that DSGRN be built, and also requires boost python to run. Note that boost python might not be built even if boost is built on Mac OS X; to remedy this type

```bash
brew install boost
brew install boost-python
```


To use the iPython Notebook feature we recommend a python distribution such as Anaconda. This has a simple GUI installer on Mac OS X. Also required is graphviz; on Mac OS X this may be obtained with: 

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
print DSGRN.yay()
```

