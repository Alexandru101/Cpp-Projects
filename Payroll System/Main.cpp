#include <iostream>

#include "Utilities.h"
#include "database/Database.h"
#include "registration/Registration.h"
#include "payroll/Payroll.h"

int main() {
	Database db;
	AuthManager auth; // Default admin account. To customize username and password to the constructor do this 'AuthManager auth("customUsername", "customPassword")';
	PayrollManager payroll;

	if (!db.openDatabase("payroll_management_system.db"))
		return 1;

	if (!db.SetupTables())
		return 1;

	while (true) {
		if (!auth.getLoggedInStatus()) {
			if (!auth.setupRegistration()) { break; }
		}

		if (!payroll.setupPayroll(db, auth)) { break; }
	}

	return 0;
}
