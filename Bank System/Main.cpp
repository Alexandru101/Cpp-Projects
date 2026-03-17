#include <memory>
#include "Utilities.h"
#include "registration/Registration.h"
#include "database/Database.h"
#include "banking_system/BankingSystem.h"

int main() {
	Database db;
	std::unique_ptr<AuthManager> auth;
	std::unique_ptr<BankManager> bank;
	std::vector<std::string> credentials;

	if (!db.open_database("Banking_System.db")) { return 1; }

	try { auth = std::make_unique<AuthManager>(&db); }
	catch (const std::exception& err) {
		std::cout << err.what();
		return 1;
	}

	if (!auth->setup_registration()) { return 1; }
	credentials = auth->get_user_logged_in();

	try { bank = std::make_unique<BankManager>(&db, credentials[0], credentials[1]); }
	catch (const std::exception& err) {
		std::cout << err.what();
		return 1;
	}

	if (!bank->setup_bank_system()) { return 1; }

	return 0;
}
