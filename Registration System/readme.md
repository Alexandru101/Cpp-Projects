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
Firstly we create two string variables that will be used to track the current logged in user and the users input so we can validate with the database if the user is valid or registering the user to the database then we mark it as private so to keep it safe from any other functions out of scope modifying the values.

We define two private member variables to encapsulate the user's credentials, ensuring they can only be modified or accessed through the class's internal logic. The class provides public methods to interface with the SQLite database for user authentication and registration, maintaining data integrity by preventing external scope modifications.

##

Creating a void function (named registerAccount) that will take the pointer to our database and will lock the user in a infinite while loop, this is usefull so we can keep asking the user to create an account until he finally creates a valid account that doesnt arleady exist on the database. Next we run a sql command that will check the database if the users input (username) arleady exists on the databse. If the user doesnt exist we attempt to save the new user however if it fails to save midway we print out to the console the error and ask the user to register again (eg ireterating while loop again).

##
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
