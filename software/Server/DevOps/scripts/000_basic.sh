# Emacs
apt-get -y install emacs24-nox

# Firewall
ufw allow ssh
ufw allow 80/tcp
ufw allow 443/tcp
ufw allow 25/tcp
ufw --force enable

# Time zone
echo "America/New_York" > /etc/timezone 
dpkg-reconfigure -f noninteractive tzdata
apt-get update
apt-get -y install ntp

# Swap space
fallocate -l 4G /swapfile
chmod 600 /swapfile
mkswap /swapfile
swapon /swapfile
sh -c 'echo "/swapfile none swap sw 0 0" >> /etc/fstab'
