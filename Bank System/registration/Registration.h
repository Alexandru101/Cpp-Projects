#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <vector>

#include "../database/Database.h"

class AuthManager
{
private:
	std::string username_input;
	std::string password_input;
	Database* db;
public:
	AuthManager(Database* database);
	AuthManager();

	bool setup_registration();
	void register_account();
	bool authenticate_account();
	void delete_account();

	std::vector<std::string> get_user_logged_in();
};
