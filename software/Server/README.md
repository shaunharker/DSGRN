# DSGRN Server


## <a name="getting-an-account"></a>Getting an Account

### Apply for an account

First, visit <https://dsgrn.com/Accounts> in order to apply for an account.

Once you receive an account, proceed to the next step.

### Login to the DSGRN Server

Visit <https://jupyter.dsgrn.com> to log in:

![Login](./images/Login.png)

## The Jupyter Notebook

### Welcome Screen

If you are successful, you will find yourself on a welcome screen:

![Welcome](./images/Welcome.png)

At any point we can return to the welcome screen by clicking the "Jupyter" logo in the upper left corner.

Here we use an example user named `lannister`.


### "Home" Page

We can make Jupyter point to the root folder of your user account. We call this the "Home" page. Clicking the link on the Welcome screen next to the phrase "Go to your home page" will take you there:

![Home](./images/home.png)


### <a name="jupyter-notebooks"></a>Jupyter Notebooks

Jupyter notebooks allow you to intersperse python code cells and Markdown (text) cells. The DSGRN server has a few tutorials on how to use the Python interface to DSGRN. To each them, go to the home page as described on the welcome page, click the "Tutorials" folder, and then click on one of the `.ipynb` files. For instance, here is a screenshot of the `DSGRN_GettingStarted.ipynb` notebook:

![tutorial](./images/tutorial.png)

As we can see here the user is choosing to re-evaluate all the cells to make sure the document is up to date. Cells can be edited by double clicking on them, and they can be re-evaluated by pressing shift-enter when they are in focus.

### <a name="terminal-sessions"></a>Terminal Sessions

A very useful feature of Jupyter is that it lets you open up a terminal session. To do this we go to the home page, and then click "New" then "terminal":

![terminal-start](./images/terminal_start.png)

This leads us to a terminal sessions (where we've played around some):

![terminal](./images/terminal.png)

## <a name="dsgrn-website"></a>Personal DSRGN Website

Going back to the Welcome page (click the Jupyter logo) we can click on the link to go to the website:

![website](./images/webpage.png)

 From here we see there are six options of what to do:

### Source Code Visualization
 This is a neat data visualization (using D3) of the C++ code source underlying the DSGRN engine. The top level "bubbles" correspond to the major classes in DSGRN. If we click on these bubbles they magnify and we have the opportunity to click on the individual methods of the class. This results in a source code listing. For instance:
 
![source](./images/source.png)
 
 
### Jupyter Notebook 
... which takes us back to the welcome page.

### Databases
...  which lets us explore associated global dynamics for networks after we have computed with DSGRN. A tutorial in the Jupyter notebook walks us through constructing such a database; after computing it, we will be able to view the results in here.

The first page shows all networks for which there are computed databases. Initally there is only one which is included as an example:

![Networks](./images/networks.png)

Clicking on the panel for the network of interest we get details about the dynamical database. In particular, we are shown a list a Morse graphs in order of frequency ranking (i.e. Morse graphs associated with more parameter nodes are shown first):

![MorseGraphs](./images/morsegraphs.png)

We can scroll down some and see increasingly complicated (but less frequent) Morse graphs:

![MorseGraphs](./images/moremorsegraphs.png)

If we click on a Morse graph then we are shown a list of integers which index the parameter nodes (i.e. parameter indices) associated with the clicked-on Morse graph:

![Parameters](./images/parameters.png)

If we click on one of these parameter indices then we get more detailed information about the dynamics, and also a list of algebraic inequalities which describe the region in parameter space corresponding to the parameter node:

![DomainGraph](./images/domaingraph.png)


### Network Builder
... a GUI for building networks with a point and click interface. 

![builder](./images/networkbuilder.png)

The specification file can be used as input for the analysis done by DSGRN. Instructions on the use of this tool are included on the page (you must scroll down to see them).

### Documentation
... the DSGRN documentation, which contains technical information about DSGRN, images for installing DSGRN on your own machines, and tutorials for the command-line only version of DSGRN.

![docs](./images/dsgrndoc.png)

### Github
... and the Github repository for DSGRN

![github](./images/github.png)

## System Architecture

The DSGRN server is an Ubuntu Linux server which is hosting a web server and a JupyterHub server. The JupyterHub authentication is based on actual user accounts; each user has a folder in the system called 

    /home/username

This is the directory one finds themselves in when they start a terminal instance in the Jupyter Notebook environment. The directory initially contains two folders:

    /home/username/notebooks/
    /home/username/public_html/

The former contains the tutorial and Welcome notebooks we saw in the Jupyter notebook system. The latter contains the user's website, which can be viewed at

<https://dsgrn.com/~username>
   
Actions in the JupyterHub terminals can be used to make changes in the `public_html` page and thus change the content of the website. For example we will compute databases which will be posted and then can be viewed on the website as described earlier.

 


