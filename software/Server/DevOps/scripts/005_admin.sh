# Create Admin user
useradd $admin_username -s /bin/bash -m -k /var/www/html/Accounts/Skeleton 
sed -i "s/username/$admin_username/g" /home/$admin_username/notebooks/Welcome.ipynb
echo "$admin_username:$admin_password" | chpasswd
gpasswd -a $admin_username sudo
sed -i 's/PermitRootLogin yes/PermitRootLogin no/g' /etc/ssh/sshd_config
mkdir -p /home/$admin_username/.ssh
cp -rf ~/.ssh /home/$admin_username
chmod 700 /home/$admin_username/.ssh
chmod 700 /home/$admin_username/.ssh/authorized_keys
chown -R $admin_username /home/$admin_username
sudo bash -c 'echo "'"$admin_username"' ALL=(ALL) NOPASSWD: ALL" | (EDITOR="tee -a" visudo)'
service ssh restart
