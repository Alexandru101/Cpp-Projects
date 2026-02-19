#include <iostream>
#include "Utilities.h"

// ----- //
// Utils //
// ----- //

void Utils::ClearInputBuffer() {
	if (std::cin.fail())
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Utils::Pause() {
	std::cout << "Press Enter To Continue . . .";
	std::cin.get();
}

void Utils::formatDate(std::string& date) {
	date = date.substr(6, 4) + "-" + date.substr(3, 2) + "-" + date.substr(0, 2);
}

// ------- //
// dbUtils //
// ------- //

bool dbUtils::CreateTable(sqlite3* db, const std::string& tableName, const std::string& columns) {
	std::string SQL = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + columns + ");";

	int response = sqlite3_exec(db, SQL.c_str(), nullptr, nullptr, nullptr);
	if (response != SQLITE_OK) {
		std::cerr << "\nFailed to create database table " << tableName << ": " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	return true;
}
