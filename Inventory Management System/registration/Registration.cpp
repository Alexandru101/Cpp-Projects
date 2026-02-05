#include <iostream>
#include "Registration.h"

AuthManager::AuthManager() : currentUser{ "" }, isLoggedIn{ false } {};
AuthManager::~AuthManager() {};

bool AuthManager::setup_user_registration(Database& db)
{
	int userInput;

	while (true)
	{
		dbUtils::registrationMenu();
		if (!(std::cin >> userInput))
		{
			std::cout << "\nInvalid Input: Please try again!\n";
			dbUtils::ClearInputBuffer();

			continue;
		}

		dbUtils::ClearInputBuffer();

		if (userInput < 1 || userInput > 4)
		{
			std::cout << "\nInvalid Number!\n";
			continue;
		}

		if (userInput == 1)
		{
			if (AuthManager::LogginUser(db))
				return true;
		}
		else if (userInput == 2)
		{
			AuthManager::RegisterUser(db);
		}
		else if (userInput == 3)
		{
			AuthManager::DeleteAccount(db);
		}
		else if (userInput == 4)
		{
			std::cout << "\nExiting Program . . .\n";
			break;
		}
	}

	return false;
}

bool AuthManager::LogginUser(Database& db)
{
	std::string username, password;

	std::cout << "\nUsername: ";
	std::getline(std::cin, username);

	std::cout << "Password: ";
	std::getline(std::cin, password);

	bool userValidated = db.ValidateUser(username, password);
	if (userValidated)
		std::cout << "\nCredentials Validated\n";
	else
		std::cout << "\nInvalid Credentials: Please try again!\n";

	currentUser = username;
	isLoggedIn = userValidated;

	return userValidated;
}

void AuthManager::RegisterUser(Database& db)
{
	std::string username, password;

	std::cout << "\nUsername: ";
	std::getline(std::cin, username);

	std::cout << "Password: ";
	std::getline(std::cin, password);

	bool accountRegisterd = db.InsertUser(username, password);
	if (accountRegisterd)
		std::cout << "\nAccount Registerd Successfully\n";
}

void AuthManager::DeleteAccount(Database& db)
{
	std::string username, password;

	std::cout << "\nUsername: ";
	std::getline(std::cin >> std::ws, username);

	std::cout << "Password: ";
	std::getline(std::cin >> std::ws, password);
	
	bool accountDeleted = db.RemoveUser(username, password);
	if (accountDeleted)
		std::cout << "\nAccount Successfully Deleted\n";
}

void AuthManager::LogOut() { isLoggedIn = false; }
std::string AuthManager::get_current_user() const { return currentUser; }
bool AuthManager::get_login_status() const { return isLoggedIn; }
