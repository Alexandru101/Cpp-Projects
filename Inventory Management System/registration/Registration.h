#pragma once

#include <string>
#include "../db/Database.h"
#include "../Utilities.h"

class AuthManager
{
private:
	std::string currentUser;
	bool isLoggedIn;
public:
	AuthManager();
	~AuthManager();

	bool setup_user_registration(Database& db);

	bool LogginUser(Database& db);
	void RegisterUser(Database& db);
	void DeleteAccount(Database& db);
	void LogOut();

	std::string get_current_user() const;
	bool get_login_status() const;
};
