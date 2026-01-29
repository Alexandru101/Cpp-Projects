# Student-Database-Management-System Documentation


## Things to know
First I will explain the header file (named Main.h) then we will go into the main cpp file (named Main.cpp).

This is my second time using the sqlite3 library so I am pretty new when it comes to SQL and struggled a lot finding how the SQL scripts work but generally my goal was to just get this project finished and not worry too much about optimizations or cleaner code. Now that I am finished I can confidently say that I can code a basic database with ease however if you are trying to use this project, know there are many things you probably could still optimize and improve the structure aswell as fixing some code.

## Step 1: Including Necessary Libraries
### Main.h Document Below
```
#pragma once // Header guard is not a library but required to protect the header file from creating duplicates

#include <iostream>
#include <limits>
#include <string>
#include "sqlite3.h"
```

## Step 2: Clearing Input Stream & Leftover Whitespaces
Clears the error state and discards the remaining input buffer. This ensures the program recovers from invalid input (like letters in a numeric field) and prevents subsequent inputs from being skipped by leftover newline characters.
```
void ClearInputBuffer()
{
	if (std::cin.fail())
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
```

## Step 3: Database Setup
We are using the provided function sqlite3_open to open an existing database however if its not found sqlite3 will then automatically create one and open it, We open the database so we can start writing and editing the file as we please.

Firstly we will need three tables that will contain the data that will be saved which is the user login information so we can loggin the user to manage the database or register a new user its important to keep track of this so the user can log back in after exiting the program and prefferably seperate to the database logins to prevent any mix up with the data however this is totally prefrence, The students login information which will be used to track all the students within the database and will be required when we do a student search lookup, The courses information which will be used to track all the existing courses of the database and allow the adming (user logged in database) to add a new course of remove an existing course. Note we also provide the option to display the students and courses in the database

We use the provided function sqlite3_open to open an existing database however, if it is not found, SQLite will automatically create and open a new one. We open the database so we can start writing to and editing the file as we please.

Firstly, we will need three tables to contain the saved data. The first is User Login Information, used to log in the user to manage the database or register a new account. It is important to keep track of this so the user can log back in after exiting the program. This is kept separate from other data to prevent any mix-ups, though this is personal preference. Second is the Student Information, which will be used to track all students within the database and is required for student search lookups. Finally, the Courses Information tracks all existing courses and allows the admin (the logged-in user) to add or remove courses. Note that we also provide options to display the students and courses currently in the database.

### Structure Of The Database
USERS: ID --> USERNAME, PASSWORD

STUDENTS: ID --> NAME, AGE, PLACE_OF_BIRTH, RELIGION

COURSES: ID --> COURSE, PASSGRADE
```
void setupDatabase(sqlite3*& db)
{
	int response = sqlite3_open("Studen_Database_Manager.db", &db);
	if (response != SQLITE_OK)
	{
		std::cerr << "\nFailed to open the database. Reason: " << sqlite3_errmsg(db) << '\n';
		sqlite3_close(db);
		exit(response);
	}

	const char* usersSQL = "CREATE TABLE IF NOT EXISTS USERS(ID INTEGER PRIMARY KEY AUTOINCREMENT, USERNAME TEXT NOT NULL UNIQUE, PASSWORD TEXT NOT NULL);";
	char* errMsg = nullptr;
	response = sqlite3_exec(db, usersSQL, nullptr, nullptr, &errMsg);
	if (response != SQLITE_OK)
	{
		std::cerr << "USERS SQL ERROR: " << errMsg << '\n';
		sqlite3_free(errMsg);
		sqlite3_close(db);
		exit(1);
	}

	const char* studentsSQL = "CREATE TABLE IF NOT EXISTS STUDENTS (ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL UNIQUE, AGE INTEGER, PLACE_OF_BIRTH TEXT, RELIGION TEXT);";
	response = sqlite3_exec(db, studentsSQL, nullptr, nullptr, &errMsg);
	if (response != SQLITE_OK)
	{
		std::cerr << "STUDENTS SQL ERROR: " << errMsg << '\n';
		sqlite3_free(errMsg);
		sqlite3_close(db);
		exit(1);
	}

	const char* coursesSQL = "CREATE TABLE IF NOT EXISTS COURSES (ID INTEGER PRIMARY KEY AUTOINCREMENT, COURSE TEXT NOT NULL UNIQUE, PASSGRADE INTEGER);";
	response = sqlite3_exec(db, coursesSQL, nullptr, nullptr, &errMsg);
	if (response != SQLITE_OK)
	{
		std::cerr << "COURSES SQL ERROR: " << errMsg << '\n';
		sqlite3_free(errMsg);
		sqlite3_close(db);
		exit(1);
	}
}
```

## Step 4: Displaying Menu-Navigation Options To The Console
```
void showMenu()
{
	std::cout << "\n1 = Login\n";
	std::cout << "2 = Register\n";
	std::cout << "3 = Exit\n";
	std::cout << "Input: ";
}

void showDatabaseMenu()
{
	system("cls");
	std::cout << "1 = Search Student\n";
	std::cout << "2 = Add Student\n";
	std::cout << "3 = Remove Student\n";
	std::cout << "4 = Display Students\n";
	std::cout << "5 = Display Courses\n";
	std::cout << "6 = Add Course\n";
	std::cout << "7 = Remove Course\n";
	std::cout << "8 = Search Course\n";
	std::cout << "9 = Log Out\n";
	std::cout << "10 = Exit\n";
	std::cout << "\nInput: ";
}
```

-----------------------------------------------------------------------------------------------------------------------------
