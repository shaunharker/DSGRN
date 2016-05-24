# DSGRN Python Web interface

Shaun Harker
2016-05-23
 
## Overview

This project is to assist in creating a server which hosts temporary Jupyter notebooks which are capable of importing the DSGRN python module and the graphviz module.

The approach is provided by the "tmpnb" Docker project. By default this uses an underlying Docker image `jupyter/minimal-notebook`. We alter that image so that DSGRN and graphviz are available, creating a new Docker image `sharker/jupyter-dsgrn`.

We also use node.js in order to forward the port from HTTP requests to the port used by the "tmpnb" program.

## Dependencies

### Docker

Install docker using their online instructions.

Then:

Create the Docker VM:
```bash
docker-machine create --driver virtualbox default
```

If the Docker VM has already been created but is not running:
```bash
docker-machine start default
```

Connect shell to default machine
```bash
eval "$(docker-machine env default)"
```
### Node.js
Install node.js (e.g. `brew install nodejs` on Mac OS X)

### Forever
Install "forever" for node.js: `npm install forever -g`

## Build the docker image

```bash
docker build -t sharker/jupyter-dsgrn jupyter-dsgrn
```

For more information on Docker visit their website. (e.g. for Mac OS X: https://docs.docker.com/engine/installation/mac/ )

## Start the notebook server

```bash
./StartNotebooks.sh
```

If necessary use the proxy pipe

```bash
forever start proxy/proxy.js 8080 $(docker-machine ip):8000
```

Now users should be able to access their temporary notebooks at `http://your.website.com:8080`.

You can also use port 80 instead of 8080, but the server needs to have the correct permissions to work. In this case the `:8080` suffix on the website would be unnecessary.

## Stopping the notebook server

To stop the notebook server, type
```bash
./StopNotebooks.sh
```

To stop the node.js proxy server, type
```bash
forever list
```
and identify the id of your proxy server. Then:
```bash
forever stop <id>
```
