#pragma once

#include <iostream>
#include <string>

#include "sqlite3.h"
#include "../Utilities.h"

class Database
{
private:
	sqlite3* db;
public:
	Database();
	~Database();

	bool open_database(const std::string& fileName);
	bool setup_tables();

	// Registration System Use //
	bool insert_user(const std::string& username, const std::string& password);
	bool validate_user(const std::string& username, const std::string& password);
	bool delete_user(const std::string& username, const std::string& password);

	// Bank System Use //
	double get_account_balance(const std::string& username, const std::string& password);
	bool deposit_amount(const double& value, const std::string& username, const std::string& password);
	bool withdraw_amount(const double& value, const std::string& username, const std::string& password);
	bool validate_withdrawl_amount(const double& value, const std::string& username, const std::string& password);
	bool transfer_funds(const std::string& source_account, const std::string& target_account, const double& transfer_amount);
};
