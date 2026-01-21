# Registration-System Documentation

![License](https://github.com/maxcomuk/Cpp-Projects/blob/main/LICENSE)

### Main Goal
Using the sqlite3 library, create a local database that will be used to store the all the registers users and allow users to log in to their accounts by verifiying if their account exists and that their password is valid. For this project we need to execute sql commands that will check the users data within the database and save data however as I am not an expert on sql commands and only now have learnt how to atleast make this project work now after finishing it, I will be using the commands from google and some help from AI.

## Step 1: Including Libraries
Import both sqlite3.h and sqlite3.c to your project from --> [Download Link](https://sqlite.org/2026/sqlite-amalgamation-3510200.zip)

Once you have both libraries and your compiler doesnt error when you try to build your cpp file with the sqlite3.h included then it is working
```
#include <iostream>
#include <string>
#include <limits>
#include "sqlite3.h"
```

## Step 2: Clearing Input Stream
Clearing the input stream of any leftover whitespaces and any console errors
```
void clearInputStream()
{
	if (std::cin.fail())
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
```

## Step 3: Prompting User With Registration Decision
Locking the user in a infinite while loop until the user has finished selecting an option.

We take a bool (named menu_navigation) to determain wether the user has just started the program or is logged in to his account. Then we prompt the user with the context-specific menu: if logged in, they can switch accounts or view details; otherwise, they are limited to initial login and registration, Depending on the users input we assign the decision and exit the loop.
```
void registrationType(int& decision, bool menu_navigation)
{
	while (true)
	{
		if (menu_navigation)
		{
			std::cout << "1 = Login Into Another Account || 2 = Register Another Account || 3 = Get Account Details\n";

			if (std::cin >> decision && (decision == 1 || decision == 2 || decision == 3))
				break;
		}
		else
		{
			std::cout << "\n1 = Login Account || 2 = Register An Account\n";

			if (std::cin >> decision && (decision == 1 || decision == 2))
				break;
		}

		std::cout << "\nInvalid Input\n";
		clearInputStream();
	}
}
```

## Step 4: Creating UserData Class
Firstly, we create two private members that will be used to track the current logged in user or get the users input then we validate with the database if the user is valid to register an account to the database or log the user into his account then we mark it as private so to keep it safe from any other functions out of scope modifying the values.

##

Secondly, We create a void function (named registerAccount) that will take the pointer to our database and will lock the user in a infinite while loop, this is usefull so we can keep asking the user to create an account until he finally creates a valid account that doesnt arleady exist on the database. Next we run a sql command that will check the database if the users input (username) arleady exists on the databse. If the user doesnt exist we attempt to save the new user however if it fails to save midway we print out to the console the error and ask the user to register again (eg ireterating while loop again).

Secondly, we define a void function named registerAccount which accepts a pointer to our database. This function utilizes an infinite loop to repeatedly prompt the user for credentials until a valid account is successfully created.

Within the loop, we execute a SQL query to check if the provided username already exists in the database. If the username is available, the system attempts to save the new user however if a failure happens mid-way then the function catches the exception, displays the error message to the console, and restarts the registration prompt.

##

Lastly we define another void function (named accountLogin) that will print out to the console the users current account details (username & password).
```
class UserData
{
private:
	std::string username;
	std::string password;
public:
	void registerAccount(sqlite3* DataBase)
	{
		sqlite3_stmt* stmt;

		while (true)
		{
			std::cout << "Username: ";
			std::getline(std::cin, username);

			std::cout << "Password: ";
			std::getline(std::cin, password);

			std::string checkSQL = "SELECT count(*) FROM USERS WHERE USERNAME = '" + username + "';";
			sqlite3_prepare_v2(DataBase, checkSQL.c_str(), -1, &stmt, NULL);
			sqlite3_step(stmt);

			int count = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
			if (count > 0)
			{
				std::cout << "\nInvalid Username: Account arleady exists\n";
				continue;
			}

			std::string saveSQL = "INSERT INTO USERS (USERNAME, PASSWORD) VALUES ('" + username + "', '" + password + "');";
			char* errorMsg;
			int saveResponse = sqlite3_exec(DataBase, saveSQL.c_str(), NULL, 0, &errorMsg);
			if (saveResponse != SQLITE_OK)
			{
				std::cerr << errorMsg;
				sqlite3_free(errorMsg);
			}
			else
			{
				std::cout << "\nUser registerd successfully\n";
				break;
			}
		}
	}

	void accountLogin(sqlite3* DataBase)
	{
		sqlite3_stmt* stmt;

		while (true)
		{
			std::cout << "Username: ";
			std::getline(std::cin, username);

			std::cout << "Password: ";
			std::getline(std::cin, password);

			const char* loginSQL = "SELECT count(*) FROM USERS WHERE USERNAME = ? AND PASSWORD = ?;";
			sqlite3_prepare_v2(DataBase, loginSQL, -1, &stmt, NULL);
			sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
			sqlite3_step(stmt);

			int count = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
			if (count == 0)
			{
				std::cout << "\nInvalid Credentials Try Again!\n";
			}
			else
			{
				std::cout << "\nUser Logged In Sucessfully\n";
				break;
			}
		}
	}

	void getAccountDetails()
	{
		std::cout << "Username: " << username << '\n';
		std::cout << "Password: " << password << '\n';
		std::cout << "\nPress Enter To Continue\n";
		std::cin.get();
	}
};
```


## Step 5: Main Entry Point (main logic)
Defining a sqlite3 pointer that will attempt to open the database with the chosen name if it fails to find it then it will auto create one however if it fails to open or create a file we will then warn the user that the database failed and will early exit the program.

Once we have opened the database we then use a SQL command that will get all the users within the database and if it doesnt exist it will auto create a template for future saves. The template we will use for this program will be (ID --> Username, Password). Note that in the SQL command we instruct the database to automatically create the ID for new users and it will go from this order (1, 2, 3 . . .).

We welcome the user with a optional prompt then call the registrationType function and give it the direct refrence of the decision which we will use to later check if the user has chosen to loggin to his account, register an account or get his account details if he is logged in.

It is important we close the database connection right before exiting the program so that we free memory and avoid any corruptions (it also allows other programs to use the database file as it wont be locked to this program). Note if we dont do this and try to reopen the program there could be issues when attempting to establish a connection with the database again.

```
int main()
{
	sqlite3* DataBase;
	int response = sqlite3_open("Registration-Database", &DataBase);
	if (response != SQLITE_OK)
	{
		std::cout << "Database Failure!\n";
		return 1;
	}

	const char* setupSQL = "CREATE TABLE IF NOT EXISTS USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT, USERNAME TEXT NOT NULL, PASSWORD TEXT NOT NULL);";
	char* sqlError;
	sqlite3_exec(DataBase, setupSQL, NULL, 0, &sqlError);

	UserData user;
	int decision;

	std::cout << "**** Welcome to the registration system ****\nPress enter to begin . . .\n";
	std::cin.get();

	registrationType(decision, false);
	clearInputStream(); // Clearing Input Stream

	while (true)
	{
		if (decision == 1)
			user.accountLogin(DataBase);
		else if (decision == 2)
			user.registerAccount(DataBase);
		else
			user.getAccountDetails();

		system("cls");

		std::cout << "\t\t\t\t**** Menu Navigation Settings ****\n";
		registrationType(decision, true);
		clearInputStream();
	}

	sqlite3_close(DataBase);

	return 0;
}
```

# Installation Process
-- Copy & Paste Method --
##
1. download the sqlite3 amalgamation file from --> [Click Me](https://sqlite.org/2026/sqlite-amalgamation-3510200.zip).

2. extract the file using winrar or your prefferd tool and include both of these files (sqlite3.c & sqlite3.h) into your project (visual studio 2022) and if you are using visual studio 2022 then right click on the file inside your project files list after you have dragged and dropped it here and press include (this includes the file to the project and allows us to acess it). Note you must do this for both files (.c & .h file).

3. Simply copy the whole of Main.cpp file and then paste it in your prefferd ide to compile

-- File Download Method --
##
1. Download Main.cpp then open it with your prefferd ide to compile it

2. download the sqlite3 amalgamation file from --> [Click Me](https://sqlite.org/2026/sqlite-amalgamation-3510200.zip).

3. extract the file using winrar or your prefferd tool and include both of these files (sqlite3.c & sqlite3.h) into your project (visual studio 2022) and if you are using visual studio 2022 then right click on the file inside your project files list after you have dragged and dropped it here and press include (this includes the file to the project and allows us to acess it). Note you must do this for both files (.c & .h file).

# Ide I used [Visual studio 2022](https://www.techspot.com/downloads/7493-visual-studio-2022.html)
