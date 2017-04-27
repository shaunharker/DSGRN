current_dir=`pwd`

# Start Server
service postfix restart
service apache2 restart
cd ~/jupyterhub
nohup jupyterhub > /dev/null &

cd $current_dir
