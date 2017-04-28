#!/bin/bash

if [ $# -ne 4 ]; then
  echo "Must be run as root."
  echo "./install.sh host_name admin_username admin_password admin_email"
  exit 1
fi

host_name=$1
admin_username=$2
admin_password=$3
admin_email=$4

source scripts/000_basic.sh
source scripts/001_webserver.sh
source scripts/002_mail.sh
source scripts/003_software.sh
source scripts/004_startup.sh
source scripts/005_admin.sh
