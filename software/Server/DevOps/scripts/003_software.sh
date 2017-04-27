current_dir=`pwd`
# Dependencies
apt-get -y install npm cmake sqlite3 libsqlite3-dev npm nodejs-legacy python python3 python3-pip libboost-all-dev

# JupyterHub
npm install -g configurable-http-proxy
pip3 install jupyterhub   
pip3 install --upgrade notebook
mkdir -p ~/jupyterhub
echo "c.JupyterHub.ssl_cert = '/etc/letsencrypt/live/$host_name/fullchain.pem'" > ~/jupyterhub/jupyterhub_config.py
echo "c.JupyterHub.ssl_key = '/etc/letsencrypt/live/$host_name/privkey.pem'" >> ~/jupyterhub/jupyterhub_config.py
echo "c.Spawner.args = ['--NotebookApp.default_url=/notebooks/notebooks/Welcome.ipynb']" >> ~/jupyterhub/jupyterhub_config.py

# Python2 instead of Python3
apt-get -y install python-pip
python2 -m pip install --upgrade ipykernel 
python2 -m ipykernel install 
mv /usr/local/lib/python3.5/dist-packages/ipykernel /usr/local/lib/python3.5/dist-packages/dead_ipykernel

# Graphviz
pip install graphviz
apt-get -y install graphviz

# Cluster-delegator
git clone https://github.com/shaunharker/cluster-delegator.git
cd cluster-delegator
sed -i 's/--test//g' install.sh
./install.sh

# DSGRN
cd ~
git clone https://github.com/shaunharker/DSGRN.git
cd DSGRN
sed -i 's/--test//g' install.sh
./install.sh
cd software/Python
./install.sh
cd modules
python setup.py install
cp ~/DSGRN/software/Signatures/bin/Signatures /usr/local/bin

# Website
cd $current_dir
cp -rf ../* /var/www/html 
cd /var/www/html/Accounts
./setup.sh
chown -R www-data /var/www/html/Accounts/
chown root /var/www/html/Accounts/add_user.sh
chown root /var/www/html/Accounts/remove_user.sh
cp /var/www/html/Accounts/add_user.sh /usr/bin/add_user.sh
cp /var/www/html/Accounts/remove_user.sh /usr/bin/remove_user.sh

cd $current_dir
