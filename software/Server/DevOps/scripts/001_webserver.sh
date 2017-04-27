# Web server
apt-get -y install apache2
apt-get -y install libapache2-mod-php
apt-get -y install apache2-suexec-custom
apt-get -y install php-sqlite3  
a2enmod userdir
a2enmod php7.0
a2enmod proxy_wstunnel
a2enmod proxy_http
a2enmod ssl
a2enmod rewrite

# SSL certs
service apache2 reload
service apache2 restart
apt-get -y install python-letsencrypt-apache 
letsencrypt --apache --non-interactive --agree-tos --email $admin_email -d $host_name -d www.$host_name -d jupyter.$host_name
(crontab -l ; echo "23 4,16 * * * letsencrypt renew") | crontab

# Reconfigure Apache
a2dissite 000-default.conf
a2dissite 000-default-le-ssl.conf
echo '
<VirtualHost *:80>
  ServerName '"$host_name"'
  ServerAlias www.'"$host_name"'
  ServerAlias jupyter.'"$host_name"'
  ServerAdmin '"$admin_username"'@localhost
  DocumentRoot /var/www/html
  ErrorLog ${APACHE_LOG_DIR}/error.log
  CustomLog ${APACHE_LOG_DIR}/access.log combined
  RewriteEngine on
  RewriteCond %{SERVER_NAME} =jupyter.'"$host_name"' [OR]
  RewriteCond %{SERVER_NAME} =www.'"$host_name"' [OR]
  RewriteCond %{SERVER_NAME} ='"$host_name"'
  RewriteRule ^ https://%{SERVER_NAME}%{REQUEST_URI} [END,QSA,R=permanent]
</VirtualHost>
' > /etc/apache2/sites-enabled/redirect.conf
echo '
<IfModule mod_ssl.c>
  <VirtualHost *:443>
    ServerAdmin '"$admin_email"'
    ServerName '"$host_name"'
    ServerAlias www.'"$host_name"'
    DocumentRoot /var/www/html
    ErrorLog ${APACHE_LOG_DIR}/error.log
    CustomLog ${APACHE_LOG_DIR}/access.log combined
    SSLEngine on
    SSLCertificateFile /etc/letsencrypt/live/'"$host_name"'/cert.pem  
    SSLCertificateKeyFile /etc/letsencrypt/live/'"$host_name"'/privkey.pem
    SSLCertificateChainFile /etc/letsencrypt/live/'"$host_name"'/chain.pem
</VirtualHost>

<VirtualHost *:443>
  ServerName jupyter.'"$host_name"'
  ServerAdmin '"$admin_email"'
  SSLProxyEngine On
  ProxyPreserveHost On
  ProxyRequests Off
  SSLCertificateFile /etc/letsencrypt/live/'"$host_name"'/cert.pem
  SSLCertificateKeyFile /etc/letsencrypt/live/'"$host_name"'/privkey.pem
  SSLCertificateChainFile /etc/letsencrypt/live/'"$host_name"'/chain.pem
  ProxyPass / https://localhost:8000/
  ProxyPassReverse / https://localhost:8000/
  <Location ~ "/(user/[^/]*)/(api/kernels/[^/]+/channels|terminals/websocket)/?">
    ProxyPass wss://localhost:8000
    ProxyPassReverse wss://localhost:8000
  </Location>
</VirtualHost>
</IfModule>
' > /etc/apache2/sites-enabled/securehosts.conf
sed -i 's/php_admin_flag engine Off/# php_admin_flag engine Off/g' /etc/apache2/mods-enabled/php7.0.conf
