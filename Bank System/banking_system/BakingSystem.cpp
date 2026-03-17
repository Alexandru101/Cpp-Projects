#include "BankingSystem.h"

BankManager::BankManager(Database* database, const std::string& username, const std::string& password)
	: db{ database }, account_username{ username }, account_password{ password } {
	if (!db) {
		throw std::runtime_error("Error: Database Null or Invalid\n");
	}

	if (!db->validate_user(account_username, account_password)) {
		throw std::runtime_error("Error: Credentials Invalid");
	}
}

BankManager::BankManager() : db{ nullptr } {
	throw std::runtime_error("Error: Failed To Initialize Class\n");
}

bool BankManager::setup_bank_system() {
	int userInput;

	while (1) {
		system("cls");

		std::cout << "**** Bank System ****\n\n";
		std::cout << "1 = Deposit\n";
		std::cout << "2 = Withdraw\n";
		std::cout << "3 = Display Account Info\n";
		std::cout << "4 = Transfer Funds\n";
		std::cout << "5 = Exit\n";
		std::cout << "Input: ";

		if (!(std::cin >> userInput)) {
			std::cout << "\nInvalid Input\n";
			
			Utils::ClearInputBuffer();
			Utils::Pause();

			continue;
		}

		Utils::ClearInputBuffer();

		switch (userInput) {
			case 1:
				deposit();

				break;
			case 2:
				withdraw();

				break;
			case 3:
				display_account_info();
				
				break;
			case 4:
				transfer();

				break;
			case 5:
				std::cout << "\nPreparing To Close Program";
				std::this_thread::sleep_for(std::chrono::milliseconds(300));
				std::cout << "\nExiting Program . . .";
				std::this_thread::sleep_for(std::chrono::milliseconds(150));

				return false;
			default:
				std::cout << "\nInvalid Input: Number Must Be Between (1 - 6)\n";
				Utils::Pause();
		}
	}

	return false;
}

void BankManager::deposit() {
	system("cls");

	std::cout << "**** Bank System ****\n\n";
	std::cout << "Deposit Amount: ";

	double value{ 0.0 };
	if (!(std::cin >> value)) {
		std::cout << "\nInvalid Input\n";

		Utils::ClearInputBuffer();
		Utils::Pause();

		return;
	}

	Utils::ClearInputBuffer();

	std::cout << "\nChecking Users Balance . . .";
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	double original_value = db->get_account_balance(account_username, account_password);
	if (!(original_value < 0)) { std::cout << "\nSuccessfully Checked Users Balance\n"; };
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	std::cout << "\nDepositing " << value << " Into User " << account_username << " Account";
	std::this_thread::sleep_for(std::chrono::milliseconds(600));

	if (db->deposit_amount(value, account_username, account_password)) {
		std::cout << "\nSuccessfully Deposited Amount: " << value << "\n\n";

		std::cout << "Checking New Balance . . .";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		double new_value = db->get_account_balance(account_username, account_password);
		if (new_value > 0) { std::cout << "\nSuccessfully Checked New Balance"; };
		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		std::cout << "\nFinalizing Calculations . . .";
		std::this_thread::sleep_for(std::chrono::milliseconds(400));
		
		std::cout << "\n\n**** New Account Balance ****\n";
		std::cout << "Account Holder: " << account_username << '\n';
		std::cout << "Balance: " << std::fixed << std::setprecision(2)
			<< ((original_value) ? original_value : 0)
			<< " --> " << ((new_value) ? new_value : 0) << "\n\n";
	}

	Utils::Pause();
}

void BankManager::withdraw() {
	system("cls");

	std::cout << "**** Bank System ****\n\n";
	std::cout << "Withdraw Amount: ";

	double value{ 0.0 };
	if (!(std::cin >> value)) {
		std::cout << "\nInvalid Input\n";

		Utils::ClearInputBuffer();
		Utils::Pause();

		return;
	}

	Utils::ClearInputBuffer();

	std::cout << "Checking users Balance . . .";
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	double original_value = db->get_account_balance(account_username, account_password);
	if (!(original_value < 0)) { std::cout << "\nSuccessfully Checked users Balance\n"; };
	std::this_thread::sleep_for(std::chrono::milliseconds(300));

	if (!db->validate_withdrawl_amount(value, account_username, account_password)) {
		Utils::Pause();
		return;
	}

	std::cout << "\nWithdrawing " << value << " From Users " << account_username << " Current Balance " << original_value << '\n';
	std::this_thread::sleep_for(std::chrono::milliseconds(600));

	if (db->withdraw_amount(value, account_username, account_password)) {
		std::cout << "Successfully Withdrawed Amount: " << value << "\n\n";

		std::cout << "Checking New Balance . . .";
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		double new_value = db->get_account_balance(account_username, account_password);
		if (!(new_value < 0)) { std::cout << "\nSuccessfully Checked New Balance"; };
		std::this_thread::sleep_for(std::chrono::milliseconds(400));

		std::cout << "\n\n**** New Account Balance ****\n";
		std::cout << "Account Holder: " << account_username << '\n';
		std::cout << "Balance: " << std::fixed << std::setprecision(2)
			<< ((original_value) ? original_value : 0)
			<< " --> " << ((new_value) ? new_value : 0) << "\n\n";
	}

	Utils::Pause();
}

void BankManager::display_account_info() {
	system("cls");

	std::cout << "Checking Balance . . .";
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	double account_balance = db->get_account_balance(account_username, account_password);
	if (!(account_balance < 0)) { std::cout << "\nSuccessfully Checked Users Balance\n\n"; };
	std::this_thread::sleep_for(std::chrono::milliseconds(400));

	char input;
	bool show_password{ false };

	while (1) {
		render_account_info(show_password, account_balance);

		if (!(std::cin >> input)) {
			std::cout << "\nInvalid Character: Please enter a valid character between (y/n)\n";
			
			Utils::ClearInputBuffer();
			Utils::Pause();

			continue;
		}

		input = std::tolower(input);
		show_password = (input == 'n') ? false : true;

		if (!show_password) {
			std::cout << "Exit (y/n): ";
			
			if (!(std::cin >> input)) {
				std::cout << "\nInvalid Character: Please enter a valid character between (y/n)\n";

				Utils::ClearInputBuffer();
				Utils::Pause();
				Utils::ClearConsoleLinesFrom(3);

				continue;
			}

			input = std::tolower(input);
			if (input == 'y') { return; }
		}

		Utils::ClearConsoleLinesFrom(3);
	}
}

void BankManager::render_account_info(const bool& show_password, const double& balance) {
	std::cout << "**** Bank Account Information ****\n";
	std::cout << "Account Holder: " << account_username << '\n';
	std::cout << "Account Password: " << ((show_password) ? account_password : std::string(account_password.length(), '*')) << '\n';
	std::cout << "Account Balance: " << balance << "\n\n";
	std::cout << "Show Password (y/n): ";
}

void BankManager::transfer() {
	system("cls");

	std::string transfer_account_name;
	double transfer_amount;

	std::cout << "**** Bank Transfer System ****\n\n";
	std::cout << "Account Name: ";
	std::getline(std::cin, transfer_account_name);

	std::cout << "Transfer Amount: ";
	if (!(std::cin >> transfer_amount)) {
		std::cout << "\nInvalid Input\n";

		Utils::ClearInputBuffer();
		Utils::Pause();

		return;
	}

	Utils::ClearInputBuffer();

	std::cout << "Attempting To Transfer Funds .";
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	Utils::ClearConsoleLinesFrom(5);

	std::cout << "Attempting To Transfer Funds . .";
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	Utils::ClearConsoleLinesFrom(5);

	std::cout << "Attempting To Transfer Funds . . .";
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	Utils::ClearConsoleLinesFrom(5);

	if (db->transfer_funds(account_username, transfer_account_name, transfer_amount)) {
		std::cout << "Successfully Transferd " << transfer_amount << " To " << transfer_account_name << '\n';
	}

	Utils::Pause();
}
