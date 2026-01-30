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

---
## Step 5: Including Header File & Creating Authmanager Class
### Main.cpp Documentation Below
The AuthManager class handles user login and registration. In this system, the logged-in user acts as the Admin, gaining control over the student's and course's data.

We use a boolean (named userLoggedIn) to track the session state. We also provide a LogOut function to reset this state. While tracking the login state is simple here, maintaining this boolean is vital for security checks if the project expands in the future.

##

The loginUser function performs a database lookup using the loginSQL script to securely check if the input credentials match any record in the USERS table. If a match is found then we set userLoggedIn to true and break out of the "while" loop.

##

The registerUser function will insert a new user into the database USERS table using the saveSQL script, first it will check if the user's USERNAME doesnt arleady exist then it will insert the new user to the table and exit out of the loop. Goal of the program is to register the user then force the user to login to that account rather than the user being logged in straight after creating an account.

The registerUser function inserts a new user into the database's USERS table using the saveSQL script. It first checks if the provided USERNAME already exists, if the name is unique then it inserts the new user  into the table. The goal of this flow is to register the account and then require the user to manually log in, rather than logging them in automatically immediately after registration.
```
#include "Main.h"


class AuthManager
{
private:
	std::string username;
	std::string password;

	bool userLoggedIn = false;
public:
	bool logginUser(sqlite3*& db)
	{
		const char* loginSQL = "SELECT count(*) FROM USERS WHERE USERNAME = ? AND PASSWORD = ?;";
		sqlite3_stmt* login_stmt;

		std::cout << "\nUsername: ";
		std::getline(std::cin, username);

		std::cout << "Password: ";
		std::getline(std::cin, password);

		if (sqlite3_prepare_v2(db, loginSQL, -1, &login_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to prepare sqlite3: " << sqlite3_errmsg(db) << '\n';
			return false;
		}

		sqlite3_bind_text(login_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(login_stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

		int count = 0;
		if (sqlite3_step(login_stmt) == SQLITE_ROW)
		{
			count = sqlite3_column_int(login_stmt, 0);
		}

		sqlite3_finalize(login_stmt);

		if (count == 0)
		{
			std::cout << "\nInvalid Credentials: Please try again\n";
			return false;
		}
		else
		{
			std::cout << "\nLogged Into Account Successfully\n";
			userLoggedIn = true;
			return true;
		}

		return false;
	}

	void registerUser(sqlite3*& db)
	{
		const char* saveSQL = "INSERT INTO USERS (USERNAME, PASSWORD) VALUES (?, ?);";
		sqlite3_stmt* save_stmt;

		std::cout << "\nUsername: ";
		std::getline(std::cin, username);

		std::cout << "Password: ";
		std::getline(std::cin, password);

		if (sqlite3_prepare_v2(db, saveSQL, -1, &save_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to prepare sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(save_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(save_stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

		int response = sqlite3_step(save_stmt);
		if (response == SQLITE_DONE)
		{
			std::cout << "\nAccount Registered Successfully\n";
		}
		else if (response == SQLITE_CONSTRAINT)
		{
			std::cout << "\nError: Account Arleady Exists\n";
		}
		else {
			std::cerr << "Error: " << sqlite3_errmsg(db) << '\n';
		}

		sqlite3_finalize(save_stmt);
	}

	void LogOut() { userLoggedIn = false; };
};

```

## Step 6: Setting Up Database Registration
The decision integer tracks the menu option selected by the user. Upon entering the infinite while loop, we call showMenu() to display the available choices and capture the user's input.

Calling ClearInputBuffer() is essential here to clear any leftover whitespace or newline characters after the user provides their input. It is also critical for error handling, allowing the program to recover if the user enters invalid data (such as letters when a number is expected).

Once the decision is captured, we perform conditional checks to call the appropriate function from the AuthManager class based on the user's choice.

IMPORTANT: If the user chooses to exit the program, we must call sqlite3_close(db) before terminating. This ensures that database resources are freed and the file is properly unlocked so it can be accessed by other programs such as the user reopening the program.
```
void setup_user_registration(AuthManager& user, sqlite3*& db)
{
	int decision;

	while (true)
	{
		showMenu();
		if (!(std::cin >> decision && (decision > 0 && decision < 4)))
		{
			std::cout << "Invalid Input: Please try again\n";
			ClearInputBuffer();
			continue;
		}

		ClearInputBuffer();

		if (decision == 1)
		{
			if (user.logginUser(db))
				break;
		}
		else if (decision == 2)
		{
			user.registerUser(db);
		}
		else
		{
			std::cout << "\nClosing Program . . .\n";
			sqlite3_close(db);
			exit(0);
		}
	}
}
```

## Step 7: School Database Struct
The schoolHandler struct serves as the core logic engine for the student database management system. It encapsulates all database operations—such as searching, adding, and removing records—ensuring. setup_database_management will be handling all the logic and require all the functions from this struct to handle each option the user selects. 

searchUser: This function prompts for a username and performs a SQL SELECT query. It uses sqlite3_prepare_v2 to compile the query and sqlite3_bind_text to safely handle the input. If sqlite3_step finds a matching row, it retrieves the data using sqlite3_column_text and sqlite3_column_int to display the student's full profile.

addStudent: Before inserting new data, this function performs a "duplicate check" by searching for the requested username. If the student already exists, the process is aborted to prevent data redundancy. If the name is unique, it executes an INSERT INTO statement, binding the name, age, birthplace, and religion to the database.

removeStudent: Verifies if a student exists by name. If found, it executes a DELETE command to remove that student from the STUDENTS table. This prevents errors that occur when trying to delete non-existent records.

displayStudents: This function executes a broad SELECT query to grab the entire STUDENTS table. It then uses a while loop to iterate through every row, each time sqlite3_step returns SQLITE_ROW, the program prints that specific student's details until the end of the table is reached.

displayCourses: Similar to the student display, this function targets the COURSES table. It loops through the available entries and prints the subject name alongside its required passing grade. It includes a boolean check (coursesExist) to inform the user if the list is currently empty.

addCourse: This function validates the course name and passing grade provided by the user. It first checks for an existing entry to avoid duplicates. If the course is new, it prepares a SQL statement and uses sqlite3_step to finalize the insertion into the COURSES table.

removeCourse: Similar to the student removal, this function targets the COURSES table. It requires both the course name and the pass grade to confirm the exact record for deletion. It uses sqlite3_bind to safely pass these parameters into the SQL statement before executing the delete command.

searchCourse: This function allows the user to look up a specific subject. It performs a targeted SELECT query based on the course name and pass grade. If the course is found, it pulls the data from the result set using sqlite3_column functions; otherwise, it notifies the user that the course is not in the system.

```
struct schoolHandler
{
	void searchUser(sqlite3*& db)
	{
		std::string userInput, username, country, religion;
		int age;

		std::cout << "Username: ";
		std::getline(std::cin, userInput);

		const char* checkSQL = "SELECT NAME, AGE, PLACE_OF_BIRTH, RELIGION FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(check_stmt, 1, userInput.c_str(), -1, SQLITE_TRANSIENT);

		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			std::cout << "\n**Student Details**\n";
			std::cout << "Name: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Age: " << sqlite3_column_int(check_stmt, 1) << '\n';
			std::cout << "Place Of Birth: " << sqlite3_column_text(check_stmt, 2) << '\n';
			std::cout << "Religion: " << sqlite3_column_text(check_stmt, 3) << '\n';
		}
		else
		{
			std::cout << "\nStudent Not Found!\n";
		}

		sqlite3_finalize(check_stmt);
	}

	void addStudent(sqlite3*& db)
	{
		std::string username, country, religion;
		int age;

		std::cout << "Name: ";
		std::getline(std::cin, username);

		std::cout << "Age: ";
		if (!(std::cin >> age))
		{
			std::cout << "\nInvalid Input: Please try again\n";
			ClearInputBuffer();
			return;
		}

		ClearInputBuffer();

		std::cout << "Place of birth: ";
		std::getline(std::cin, country);

		std::cout << "Religion: ";
		std::getline(std::cin, religion);

		const char* checkSQL = "SELECT count(*) FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db);
			return;
		}

		sqlite3_bind_text(check_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_step(check_stmt);

		int count = sqlite3_column_int(check_stmt, 0);
		sqlite3_finalize(check_stmt);

		if (count > 0)
		{
			std::cout << "\nInvalid Username: Student Arleady Exists\n";
			return;
		}

		const char* saveSQL = "INSERT INTO STUDENTS (NAME, AGE, PLACE_OF_BIRTH, RELIGION) VALUES (?, ?, ?, ?);";
		sqlite3_stmt* save_stmt;

		if (sqlite3_prepare_v2(db, saveSQL, -1, &save_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db);
			return;
		}

		sqlite3_bind_text(save_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(save_stmt, 2, age);
		sqlite3_bind_text(save_stmt, 3, country.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(save_stmt, 4, religion.c_str(), -1, SQLITE_TRANSIENT);

		int response = sqlite3_step(save_stmt);
		if (response != SQLITE_DONE)
			std::cerr << "\nError: " << sqlite3_errmsg(db) << '\n';
		else
			std::cout << "\nStudent Registered Successfully\n";

		sqlite3_finalize(save_stmt);
	}

	void removeStudent(sqlite3*& db)
	{
		std::string username;

		std::cout << "Username: ";
		std::getline(std::cin, username);

		const char* checkSQL = "SELECT count(*) FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(check_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

		int count = 0;
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			count = sqlite3_column_int(check_stmt, 0);
		}
		sqlite3_finalize(check_stmt);

		if (count == 0)
		{
			std::cout << "\nStudent Not Found!\n";
			return;
		}

		const char* deleteSQL = "DELETE FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* delete_stmt;

		if (sqlite3_prepare_v2(db, deleteSQL, -1, &delete_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(delete_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

		if (sqlite3_step(delete_stmt) != SQLITE_DONE)
		{
			std::cout << "\nError Removing Student" << sqlite3_errmsg(db) << '\n';
		}
		else
		{
			std::cout << "\nRemoved Student Successfully\n";
		}

		sqlite3_finalize(delete_stmt);
	}

	void displayStudents(sqlite3*& db)
	{
		const char* checkSQL = "SELECT NAME, AGE, PLACE_OF_BIRTH, RELIGION FROM STUDENTS;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		std::cout << "\n**** Students List ****\n";

		bool usersExist = false;
		int counter = 0;

		while (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			usersExist = true;
			counter++;

			std::cout << "STUDENT " << counter << '\n';
			std::cout << "Name: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Age: " << sqlite3_column_int(check_stmt, 1) << '\n';
			std::cout << "Place Of Birth: " << sqlite3_column_text(check_stmt, 2) << '\n';
			std::cout << "Religion: " << sqlite3_column_text(check_stmt, 3) << '\n';
			std::cout << "---------------------------------\n\n";
		}

		if (!usersExist)
			std::cout << "\nNot Found Users\n";

		sqlite3_finalize(check_stmt);
	}

	void displayCourses(sqlite3*& db)
	{
		const char* checkSQL = "SELECT COURSE, PASSGRADE FROM COURSES;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		std::cout << "\n**** Courses List ****\n";

		bool coursesExist = false;
		int counter = 0;

		while (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			coursesExist = true;
			counter++;

			std::cout << "COURSE " << counter << '\n';
			std::cout << "Subject: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Pass Grade: " << sqlite3_column_int(check_stmt, 1) << '\n';
			std::cout << "---------------------------------\n\n";
		}

		if (!coursesExist)
			std::cout << "\nNot Found Courses\n";

		sqlite3_finalize(check_stmt);
	}

	void addCourse(sqlite3*& db)
	{
		std::string courseName;
		int passGrade;

		std::cout << "Subject: ";
		std::getline(std::cin, courseName);

		std::cout << "Pass Grade: ";
		if (!(std::cin >> passGrade))
		{
			ClearInputBuffer();

			std::cout << "\nInvalid Input: Please try again\n";
			return;
		}

		ClearInputBuffer();

		const char* checkSQL = "SELECT count(*) FROM COURSES WHERE COURSE = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(check_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);

		int count = 0;
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			count = sqlite3_column_int(check_stmt, 0);
		}

		sqlite3_finalize(check_stmt);

		if (count > 0)
		{
			std::cout << "\nCourse Already Exists!\n";
			return;
		}

		const char* saveSQL = "INSERT INTO COURSES (COURSE, PASSGRADE) VALUES (?, ?);";
		sqlite3_stmt* save_stmt;

		if (sqlite3_prepare_v2(db, saveSQL, -1, &save_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(save_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(save_stmt, 2, passGrade);

		int response = sqlite3_step(save_stmt);
		if (response != SQLITE_DONE)
			std::cout << "\nFailed To Save Course: " << sqlite3_errmsg(db) << '\n';
		else
			std::cout << "\nCourse Saved Successfully\n";

		sqlite3_finalize(save_stmt);
	}

	void removeCourse(sqlite3*& db)
	{
		std::string courseName;
		int passGrade;

		std::cout << "Course Name: ";
		std::getline(std::cin, courseName);

		std::cout << "Pass Grade: ";
		if (!(std::cin >> passGrade))
		{
			ClearInputBuffer();

			std::cout << "\nInvalid Input: Please try again\n";
			return;
		}

		ClearInputBuffer();

		const char* checkSQL = "SELECT count(*) FROM COURSES WHERE COURSE = ? AND PASSGRADE = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(check_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(check_stmt, 2, passGrade);

		int count = 0;
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
			count = sqlite3_column_int(check_stmt, 0);
		sqlite3_finalize(check_stmt);

		if (count == 0)
		{
			std::cout << "\nCourse Not Found: Please try again\n";
			return;
		}

		const char* deleteSQL = "DELETE FROM COURSES WHERE COURSE = ? AND PASSGRADE = ?;";
		sqlite3_stmt* delete_stmt;

		if (sqlite3_prepare_v2(db, deleteSQL, -1, &delete_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		sqlite3_bind_text(delete_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(delete_stmt, 2, passGrade);

		if (sqlite3_step(delete_stmt) != SQLITE_DONE)
			std::cerr << "\nFailed To Delete Course: " << sqlite3_errmsg(db) << '\n';
		else
			std::cout << "\nCourse Removed Successfully\n";

		sqlite3_finalize(delete_stmt);
	}

	void searchCourse(sqlite3*& db)
	{
		std::string courseName;
		int passGrade;

		std::cout << "Course Name: ";
		std::getline(std::cin, courseName);

		std::cout << "Pass Grade: ";
		if (!(std::cin >> passGrade))
		{
			ClearInputBuffer();

			std::cout << "\nInvalid Input: Please try again\n";
			return;
		}

		ClearInputBuffer();

		const char* checkSQL = "SELECT COURSE, PASSGRADE FROM COURSES WHERE COURSE = ? AND PASSGRADE = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db);
			return;
		}

		sqlite3_bind_text(check_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(check_stmt, 2, passGrade);

		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			std::cout << "\nSubject: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Pass Grade: " << sqlite3_column_int(check_stmt, 1) << '\n';
		}
		else
		{
			std::cout << "\nNot Found Course!\n";
		}

		sqlite3_finalize(check_stmt);
	}
};
```

## Step 8: Database Management Control Loop
"This function controls the schoolHandler struct. It implements an infinite while loop that keeps the interface active until the user decides to log out or exit the program. This loop ensures the user stays within the management system until they choose to leave.
```
void setup_database_management(schoolHandler& schoolSystem, AuthManager& user, sqlite3*& db)
{
	int decision;

	while (true)
	{
		showDatabaseMenu();
		if (!(std::cin >> decision && (decision > 0 && decision < 11)))
		{
			std::cout << "Invalid Input: Please try again\n";
			ClearInputBuffer();
			continue;
		}

		ClearInputBuffer();
		system("cls");

		if (decision == 1)
		{
			schoolSystem.searchUser(db);
		}
		else if (decision == 2)
		{
			schoolSystem.addStudent(db);
		}
		else if (decision == 3)
		{
			schoolSystem.removeStudent(db);
		}
		else if (decision == 4)
		{
			schoolSystem.displayStudents(db);
		}
		else if (decision == 5)
		{
			schoolSystem.displayCourses(db);
		}
		else if (decision == 6)
		{
			schoolSystem.addCourse(db);
		}
		else if (decision == 7)
		{
			schoolSystem.removeCourse(db);
		}
		else if (decision == 8)
		{
			schoolSystem.searchCourse(db);
		}
		else if (decision == 9)
		{
			user.LogOut();
			break;
		}
		else if (decision == 10)
		{
			std::cout << "Closing Program . . .\n";
			sqlite3_close(db);
			exit(0);
		}

		std::cout << "\nPress Enter To Continue";
		std::cin.get();
	}
}
```

## Step 9: Program Main Entry
Lastly this is where the program will start running the code and this function will be used to control all the logic.

We first instantiate both user class and schoolsystem class then setup the database. After this we enter a infinite while loop to keep the program alive and call either the user registration function or the database management function.

Note we dont actully need to close the database or return 0 as we exit the program manually inside the setup_user_registration or setup_database_management functions however if we were to expand this project or clean it up then it would be vital we handle the exiting logic inside here.

## Step 9: Program Main Entry

This is the entry point where the program starts and handles all the logic.

First, we instantiate the AuthManager and schoolHandler classes and initialize the database connection. We then enter an infinite while loop to keep the program running, alternating between the user registration/login menu and the database management system.

Note: While the code includes sqlite3_close and return 0 at the end, this is unnecessary because the program exits manually from within the sub-functions. If the project is expanded or cleaned up later, this exit logic should be centralized here and not within the sub-functions for better structure (ultimatly this is just prefrence and not required).
```
int main()
{
	AuthManager user;
	schoolHandler schoolSystem;

	std::cout << "**** Student Database Manager ****\n";

	sqlite3* DataBase = nullptr;
	setupDatabase(DataBase);

	while (true)
	{
		setup_user_registration(user, DataBase);
		setup_database_management(schoolSystem, user, DataBase);
		system("cls");
	}

	sqlite3_close(DataBase);
	return 0;
}
```

# Installation Process
First you will need to include sqlite3.c and sqlite3.h to your project (prefferd ide or what you use to compile). I have put both sqlite3 versions I have used for this project however you can also download it from the original source here --> [sqlite3 download](https://www.sqlite.org/2026/sqlite-amalgamation-3510200.zip)

- Copy & Paste Method -
1. Create two files, one will be a cpp file and the other will be a header file (Main.cpp and Main.h).
2. Copy the content of the Main.cpp and paste it into your cpp file
3. Copy the content of the Main.h and paste it into your header (.h) file
4. compile and run the program

- File Download Method -
1. Download both Main.cpp and Main.h files
2. Compile the project and run the program

Note. You must have both sqlite3.c and sqlite3.h in order for it to work as this will be required for creating the SQL database file

# Ide I used [Visual studio 2022](https://www.techspot.com/downloads/7493-visual-studio-2022.html)
