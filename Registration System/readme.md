# Registration-System Documentation

![License](https://github.com/maxcomuk/Cpp-Projects/blob/main/LICENSE)

### Main Goal
Using the sqlite3 library, create a local database that will be used to store the all the registers users and allow users to log in to their accounts by verifiying if their account exists and that their password is valid. For this project we need to execute sql commands that will check the users data within the database and save data however as I am not an expert on sql commands and only now have learnt how to atleast make this project work now after finishing it, I will be using the commands from google and some help from AI.

## Step 1: Including Libraries
Import both sqlite3.h and sqlite3.c from --> [Download Link](https://sqlite.org/2026/sqlite-amalgamation-3510200.zip)

Once you have both libraries and your compiler doesnt error when you try to build your cpp file with the sqlite3.h included then it is working
```
#include <iostream>
#include <string>
#include <limits>
#include "sqlite3.h"
```
