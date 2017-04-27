#!/bin/bash
name=$1
email=$2
user=$3
password=`cat /dev/urandom | env LC_CTYPE=C tr -dc 'a-zA-Z0-9' | fold -w 8 | head -n 1`
sudo useradd $user -s $(which bash) -m -k /var/www/html/Accounts/Skeleton 
echo $user:$password | sudo chpasswd 
sudo sed -i "s/username/$user/g" /home/$user/notebooks/Welcome.ipynb
sudo sqlite3 /var/www/html/Accounts/users.db "insert into Users values ('$1','$2','$3');"
echo "Dear $name," > temp_email.txt
echo "    Your account has been created." >> temp_email.txt
echo "    The username is $user" >> temp_email.txt
echo "    The temporary password is $password" >> temp_email.txt
echo "    Visit https://jupyter.dsgrn.com to log in.\n" >> temp_email.txt
echo "Best wishes,\n" >> temp_email.txt
echo "  dsgrn.com" >> temp_email.txt
cat temp_email.txt | mail -s 'Account Creation' $email
rm temp_email.txt
