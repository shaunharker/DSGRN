#!/bin/bash
sudo pkill -9 -u $1
sudo userdel $1
sudo rm -rf /home/$1
sudo sqlite3 /var/www/html/Accounts/users.db "delete from Users where user = '$1';"
