#!/bin/bash

sqlite3 users.db 'create table Users (name TEXT, email TEXT, user TEXT UNIQUE ON CONFLICT FAIL);'
chmod 600 users.db 
