# Jupyter Notebook with DSGRN module available
FROM jupyter/minimal-notebook
MAINTAINER Shaun Harker <sharker@math.rutgers.edu>

#########
# CMAKE #
#########
RUN \ 
  mkdir /home/jovyan/software && \
  cd /home/jovyan/software && \
  wget https://cmake.org/files/v3.5/cmake-3.5.2.tar.gz && \
  tar xvfz cmake-3.5.2.tar.gz && \
  cd cmake-3.5.2 && \
  ./configure --prefix=/home/jovyan/software && \
  make && \
  make install

#########
# BOOST #
#########
RUN \
  cd /home/jovyan/software && \ 
  wget http://downloads.sourceforge.net/project/boost/boost/1.61.0/boost_1_61_0.tar.gz && \
  tar xvfz boost_1_61_0.tar.gz && \
  cd boost_1_61_0 && \
  ./bootstrap.sh --prefix=/home/jovyan/software --with-libraries=serialization,python && \
  sed -i 's/using python : 3.5 : \/opt\/conda /using python : 3.5 : \/opt\/conda\/bin\/python : \/opt\/conda\/include\/python3.5m /g' project-config.jam  && \
  ./b2 install

###########
# sqlite3 #
###########
RUN \
  cp /opt/conda/pkgs/sqlite-3.11.1-0/include/sqlite3.h /home/jovyan/software/include && \
  cp /opt/conda/pkgs/sqlite-3.11.1-0/include/sqlite3ext.h /home/jovyan/software/include && \
  cp /opt/conda/pkgs/sqlite-3.11.1-0/lib/libsqlite3.so /home/jovyan/software/lib

ENV PATH=/home/jovyan/software/bin:$PATH
ENV BOOST_ROOT=/home/jovyan/software

#########
# DSGRN #
#########
RUN \
  cd /home/jovyan/software && \
  git clone https://github.com/shaunharker/DSGRN.git && \
  cd DSGRN && \
  git checkout python_bindings && \
  ./install.sh --prefix=/home/jovyan/software; exit 0  # ignore open-mpi based failure

#######################
# DSGRN PYTHON MODULE #
#######################
RUN \
  cd /home/jovyan/software/DSGRN/software/Python && \
  sed -i 's/2.7/3.5/g' CMakeLists.txt && \
  ./install.sh --prefix=/home/jovyan/software && \
  cp -rf /home/jovyan/software/share/DSGRN/modules/DSGRN /opt/conda/lib/python3.5/site-packages/

############
# GRAPHVIZ #
############ 
RUN \
  cd /home/jovyan/software && \
  pip install graphviz && \
  wget http://pkgs.fedoraproject.org/repo/pkgs/graphviz/graphviz-2.38.0.tar.gz/5b6a829b2ac94efcd5fa3c223ed6d3ae/graphviz-2.38.0.tar.gz && \
  tar xvfz graphviz-2.38.0.tar.gz && \
  cd graphviz-2.38.0 && \
  ./configure --prefix=/home/jovyan/software && \
  make && \
  make install

############
# NOTEBOOK #
############ 
RUN cp /home/jovyan/software/DSGRN/software/Python/doc/* /home/jovyan/work 
