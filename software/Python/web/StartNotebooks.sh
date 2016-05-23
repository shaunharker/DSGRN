#!/bin/bash
export TOKEN=$( head -c 30 /dev/urandom | xxd -p )
docker run --net=host -d -e CONFIGPROXY_AUTH_TOKEN=$TOKEN --name=proxy jupyter/configurable-http-proxy --default-target http://127.0.0.1:9999
docker run --net=host -d -e CONFIGPROXY_AUTH_TOKEN=$TOKEN --name=tmpnb -v /var/run/docker.sock:/docker.sock jupyter/tmpnb python orchestrate.py --image='sharker/jupyter-dsgrn' --command='start-notebook.sh \
            "--NotebookApp.base_url={base_path} \
            --ip=0.0.0.0 \
            --port={port} \
            --NotebookApp.trust_xheaders=True"'
# Visit your Docker host on port 8000
# Ip address = `docker-machine ip`
IP=`docker-machine ip`
echo "Visit $IP:8000"
