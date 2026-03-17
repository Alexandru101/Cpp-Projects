#include "Database.h"

Database::Database() : db{nullptr} {
}

Database::~Database() {
	if (db) { sqlite3_close(db); }
}

bool Database::open_database(const std::string& fileName) {
	int response = sqlite3_open(fileName.c_str(), &db);
	if (response != SQLITE_OK) {
		std::cerr << "\nError: Failed To Open/Create Database!\n";
		
		return false;
	}

	sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
	Database::setup_tables();
	
	return true;
}

bool Database::setup_tables() {
	std::string users_column =
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"username TEXT NOT NULL UNIQUE, "
		"password TEXT NOT NULL, "
		"balance REAL DEFAULT 0.0";

	if (!dbUtils::createTable(db, "users", users_column))
		return false;

	return true;
}

bool Database::insert_user(const std::string& username, const std::string& password) {
	const char* SQL = "INSERT INTO users (username, password) VALUES (?, ?);";
	sqlite3_stmt* insert_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &insert_stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	sqlite3_bind_text(insert_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(insert_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	int response = sqlite3_step(insert_stmt);
	sqlite3_finalize(insert_stmt);

	if (response != SQLITE_DONE) {
		const char* error_message = sqlite3_errmsg(db);

		if (response == SQLITE_CONSTRAINT)
			std::cerr << "\nError: User Arleady Exists.\n(Info) " << error_message << '\n';
		else
			std::cerr << "\nError: " << error_message << '\n';

		return false;
	}

	return response == SQLITE_DONE;
}

bool Database::validate_user(const std::string& username, const std::string& password) {
	const char* SQL = "SELECT COUNT(*) FROM users WHERE username = ? AND password = ?;";
	sqlite3_stmt* validate_stmt;

	if (sqlite3_prepare_v2(db, SQL, -1, &validate_stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	sqlite3_bind_text(validate_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(validate_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	bool user_validated{ false };
	if (sqlite3_step(validate_stmt) == SQLITE_ROW) {
		if (sqlite3_column_int(validate_stmt, 0) > 0)
			user_validated = true;
	}

	sqlite3_finalize(validate_stmt);
	return user_validated;
}

bool Database::delete_user(const std::string& username, const std::string& password) {
	const char* SQL = "DELETE FROM users WHERE username = ? AND password = ?;";
	sqlite3_stmt* delete_stmt;

	if (sqlite3_prepare_v2(db, SQL, -1, &delete_stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	sqlite3_bind_text(delete_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(delete_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	int response = sqlite3_step(delete_stmt);
	sqlite3_finalize(delete_stmt);

	if (response != SQLITE_DONE) {
		std::cerr << "\nFailed To Remove Account From Database: " << sqlite3_errmsg(db) << "\n\n";

		return false;
	}

	return true;
}

double Database::get_account_balance(const std::string& username, const std::string& password) {
	const char* SQL = "SELECT balance FROM users WHERE username = ? AND password = ?;";
	sqlite3_stmt* select_stmt;

	if (sqlite3_prepare_v2(db, SQL, -1, &select_stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';

		return -1;
	}

	sqlite3_bind_text(select_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(select_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	int response = sqlite3_step(select_stmt);
	double balance{ -1 };

	if (response == SQLITE_ROW) {
		balance = sqlite3_column_double(select_stmt, 0);
	}
	else if (response == SQLITE_DONE) {
		std::cerr << "\nUser Not Found: " << sqlite3_errmsg(db) << "\n\n";
	}
	else {
		std::cerr << "\nError: " << sqlite3_errmsg(db) << "\n\n";
	}

	sqlite3_finalize(select_stmt);

	return balance;
}

bool Database::deposit_amount(const double& value, const std::string& username, const std::string& password) {
	const char* SQL = "UPDATE users SET balance = balance + ? WHERE username = ? AND password = ?;";
	sqlite3_stmt* update_stmt;

	if (sqlite3_prepare_v2(db, SQL, -1, &update_stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	sqlite3_bind_double(update_stmt, 1, value);
	sqlite3_bind_text(update_stmt, 2, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(update_stmt, 3, password.c_str(), -1, SQLITE_STATIC);

	int response = sqlite3_step(update_stmt);
	sqlite3_finalize(update_stmt);

	if (response != SQLITE_DONE) {
		std::cerr << "\nFailed To Deposit Amount - Reason For Failure: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	return true;
}

bool Database::withdraw_amount(const double& value, const std::string& username, const std::string& password) {
	const char* SQL = "UPDATE users SET balance = balance - ? WHERE username = ? AND password = ?";
	sqlite3_stmt* update_stmt;

	if (sqlite3_prepare_v2(db, SQL, -1, &update_stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	sqlite3_bind_double(update_stmt, 1, value);
	sqlite3_bind_text(update_stmt, 2, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(update_stmt, 3, password.c_str(), -1, SQLITE_STATIC);

	int response = sqlite3_step(update_stmt);
	sqlite3_finalize(update_stmt);

	if (response != SQLITE_DONE) {
		std::cerr << "\nFailed To Withdraw Amount - Reason For Failure: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	if (sqlite3_changes(db) == 0) {
		std::cerr << "\nAttempt To Withdraw Failed. Error: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	return true;
}

bool Database::validate_withdrawl_amount(const double& value, const std::string& username, const std::string& password) {
	const char* SQL = "SELECT balance FROM users WHERE username = ? AND password = ?;";
	sqlite3_stmt* validate_stmt;

	if (sqlite3_prepare_v2(db, SQL, -1, &validate_stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';

		return false;
	}

	sqlite3_bind_text(validate_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(validate_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	int response = sqlite3_step(validate_stmt);
	if (response != SQLITE_ROW) {
		std::cerr << "\nFailed To Access Account. Possible Issues: Incorrect username/password or database issue\n";

		sqlite3_finalize(validate_stmt);
		return false;
	}

	double balance = sqlite3_column_double(validate_stmt, 0);
	sqlite3_finalize(validate_stmt);

	const double tolerance = 0.01;
	if (balance < (value - tolerance)) {
		std::cout << "\nInsufficient Amount For Withdrawl. Current Balance: " << balance << "\n\n";

		return false;
	}

	return true;
}

bool Database::transfer_funds(const std::string& source_account, const std::string& target_account, const double& transfer_amount) {
	if (source_account == target_account) {
		std::cerr << "\nCannot Transfer Funds To The Same Account!\n";

		return false;
	}

	char* errMsg = nullptr;
	if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "\nFailed To Start Transaction: " << errMsg << '\n';
		sqlite3_free(errMsg);

		return false;
	}

	const char* check_sql = "SELECT balance FROM users WHERE username = ?;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	sqlite3_bind_text(stmt, 1, source_account.c_str(), -1, SQLITE_STATIC);

	double source_balance{ 0.0 };
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		source_balance = sqlite3_column_double(stmt, 0);
	}
	else {
		std::cerr << "\nNot Found Source Account\n";
		sqlite3_finalize(stmt);
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	sqlite3_finalize(stmt);

	if (source_balance < transfer_amount) {
		std::cerr << "\nInsufficient Funds!. Current Balance: " << source_balance << " || Transfer Amount: " << transfer_amount << '\n';
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	const char* deduct_source_sql = "UPDATE users SET balance = balance - ? WHERE username = ?;";
	if (sqlite3_prepare_v2(db, deduct_source_sql, -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	sqlite3_bind_double(stmt, 1, transfer_amount);
	sqlite3_bind_text(stmt, 2, source_account.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "\nFailed To Deduct Source Account: " << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	sqlite3_finalize(stmt);

	const char* add_target_sql = "UPDATE users SET balance = balance + ? WHERE username = ?;";
	if (sqlite3_prepare_v2(db, add_target_sql, -1, &stmt, nullptr) != SQLITE_OK) {
		std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	sqlite3_bind_double(stmt, 1, transfer_amount);
	sqlite3_bind_text(stmt, 2, target_account.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		std::cerr << "\nFailed To Add Funds To Targeted Account: " << sqlite3_errmsg(db) << '\n';
		sqlite3_finalize(stmt);
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	sqlite3_finalize(stmt);

	if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
		std::cerr << "\nFailed To Commit Transaction: " << errMsg << '\n';
		sqlite3_free(errMsg);
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);

		return false;
	}

	return true;
}
