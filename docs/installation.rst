Installation
============

Dependencies
------------

* Modern C++ compiler
* cmake
* sqlite3
* Python3
* Jupyter Notebook
* Open MPI and mpi4py (optional)

Installation on macOS
---------------------

Install Modern C++:
  
.. code-block:: bash

    xcode-select --install    # Then click "install" button on dialog

Install Homebrew https://brew.sh, an open source package manager

.. code-block:: bash

    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

To install `cmake`:

.. code-block:: bash

    brew install cmake

You will also want ``python3``; both homebrew's python and Anaconda3_ have been tested.

Experience seems to indicate Anaconda3_ is the most fool-proof setup for most people.

Another option is to use homebrew python:

.. code-block:: bash

    brew install python

To use the ``Signatures`` tool, which uses MPI <https://en.wikipedia.org/wiki/Message_Passing_Interface> you also need:

.. code-block:: bash

    brew install openmpi
    pip install mpi4py

Finally,

.. code-block:: bash

    # Install DSGRN
    git clone https://github.com/shaunharker/DSGRN.git
    cd DSGRN
    ./install.sh

Installation on Linux
---------------------

On an HPC cluster it is likely modern compilers, python, and a suitable version of MPI are already installed.
However, you cannot ``pip install`` due to permissions issues. In this case one solution is to pass the ``--user`` flag:

.. code-block:: bash

    pip install mpi4py --user
    # Install DSGRN
    git clone https://github.com/shaunharker/DSGRN.git
    cd DSGRN
    ./install.sh --user

This would put the ``Signatures`` script in ``~/.local/bin``, so you may consider putting that on your ``PATH``.

On your own system you may use the package manager to install dependencies, e.g.

.. code-block:: bash

    sudo apt install libopenmpi-dev

on Ubuntu. 

Uninstalling
------------

.. code-block:: bash

    pip uninstall DSGRN

Troubleshooting
---------------

Python issues:
++++++++++++++

If python won't stop giving you trouble, I recommend trying Anaconda3_.

.. _Anaconda3: https://www.anaconda.com.

This will install into a folder named ``anaconda3`` and add a line in ``~/.bash_profile``:

.. code-block:: bash

    #added by Anaconda3 5.0.1 installer
    export PATH="/PATH/TO/anaconda3/bin:$PATH"


This will redirect command line python and pip. Note you may have to start a new ``bash`` session for the path changes to take effect (i.e. close and reopen the terminal program). This has the effect of plastering over any problems you might be having with multiple installations/permissions problems/jupyter not seeing the package/etc.

Permissions issues on macOS:
++++++++++++++++++++++++++++

If the installation gives permissions issues, oftentimes the culprit is broken permissions on the subfolders of the homebrew folder ``/usr/local``. 

First, see what

.. code-block:: bash

    brew doctor


says. A common fix is:

.. code-block:: bash
    
    sudo chown -R $(whoami) $(brew --prefix)/*


If it still doesn't work after this, then you might try uninstalling and reinstalling homebrew.

To uninstall homebrew:

.. code-block:: bash

    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/uninstall)"


Or ``sudo`` if it gives issues:

.. code-block:: bash

    sudo ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/uninstall)"


To install homebrew (don't use ``sudo`` here!):

.. code-block:: bash

    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"


Linux permissions issues:
+++++++++++++++++++++++++

For missing dependencies, you'll need to contact your system admin.

For python modules, you can pass the ``--user`` flag:

.. code-block:: bash
    
    # Install DSGRN
    git clone https://github.com/shaunharker/DSGRN.git
    cd DSGRN
    ./install.sh --user


Python/Jupyter Integration issues:
++++++++++++++++++++++++++++++++++

If the package installs but it is not visible in jupyter, the likely problem is that the jupyter python kernel is not the same python for which pychomp was installed. That is, you may have multiple pythons on your system.

You can try to confirm this by typing

.. code-block:: bash
    
    which python
    which pip
    which jupyter


Possible fixes include steps such as 

1. Checking/changing your environmental variable ``PATH`` in ``~/.bash_profile`` or ``.bashrc``
2. Uninstalling python and jupyter, then reinstalling python then jupyter
3. Plastering over with anaconda3
4. Googling for answers until 3AM

You suspect you have an old install of DSGRN conflicting:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

In python, type

.. code-block:: python

    import DSGRN
    print(DSGRN.__name__)


This will tell you the path to the DSGRN the python module loader used, and you can check if it correct.
