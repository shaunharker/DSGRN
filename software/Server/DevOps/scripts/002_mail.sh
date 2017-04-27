# Mail
debconf-set-selections <<< "postfix postfix/mailname string $host_name"
debconf-set-selections <<< "postfix postfix/main_mailer_type string 'Internet Site'"
apt-get -y install mailutils
echo "
smtpd_use_tls=yes
smtp_use_tls=yes
smtpd_tls_protocols = !SSLv2, !SSLv3
smtp_tls_protocols = !SSLv2, !SSLv3
smtpd_tls_ciphers = high
smtp_tls_ciphers = high                                                                         
smtpd_tls_cert_file = /etc/letsencrypt/live/$host_name/fullchain.pem
smtpd_tls_key_file = /etc/letsencrypt/live/$host_name/privkey.pem
smtp_tls_cert_file = /etc/letsencrypt/live/$host_name/fullchain.pem
smtp_tls_key_file = /etc/letsencrypt/live/$host_name/privkey.pem
" >> /etc/postfix/main.cf
