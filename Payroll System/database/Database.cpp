#include <iostream>
#include "Database.h"

Database::Database() : db{ nullptr } {}

Database::~Database() {
	if (db) {
		sqlite3_close(db);
		db = nullptr;
	}
}

bool Database::openDatabase(const std::string& fileName) {
	int response = sqlite3_open(fileName.c_str(), &db);
	if (response != SQLITE_OK) {
		std::cerr << "Failed To Open Database!\n\nError: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
	return true;
}

bool Database::SetupTables() {
    std::string employee_column =
        "id INTEGER PRIMARY KEY, "
        "first_name TEXT NOT NULL, "
        "last_name TEXT NOT NULL, "
        "department TEXT, "
        "employment_type TEXT, "
        "hourly_rate REAL NOT NULL, "
        "hire_date DATE NOT NULL, "
        "is_active INTEGER DEFAULT 1";

    std::string time_entries_column =
        "id INTEGER PRIMARY KEY, "
        "employee_id INTEGER NOT NULL, "
        "date_worked DATE NOT NULL, "
        "hours_worked REAL NOT NULL, "
        "UNIQUE(employee_id, date_worked), "
        "FOREIGN KEY(employee_id) REFERENCES employees(id) ON DELETE CASCADE";

    std::string pay_periods_column =
        "id INTEGER PRIMARY KEY, "
        "start_date DATE NOT NULL, "
        "end_date DATE NOT NULL, "
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "processed_at DATETIME, "
        "CHECK(start_date < end_date)";

    std::string payroll_column =
        "id INTEGER PRIMARY KEY, "
        "employee_id INTEGER NOT NULL, "
        "pay_period_id INTEGER NOT NULL, "
        "gross_pay REAL NOT NULL, "
        "net_pay REAL NOT NULL, "
        "UNIQUE(employee_id, pay_period_id), "
        "FOREIGN KEY(employee_id) REFERENCES employees(id) ON DELETE CASCADE, "
        "FOREIGN KEY(pay_period_id) REFERENCES pay_periods(id) ON DELETE CASCADE";

    if (!dbUtils::CreateTable(db, "employees", employee_column))
        return false;

    if (!dbUtils::CreateTable(db, "time_entries", time_entries_column))
        return false;

    if (!dbUtils::CreateTable(db, "pay_periods", pay_periods_column))
        return false;

    if (!dbUtils::CreateTable(db, "payroll", payroll_column))
        return false;

    return true;
}

bool Database::addPayPeriod(const std::string& startDate, const std::string& endDate) {
    const char* SQL = "INSERT INTO pay_periods (start_date, end_date) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_text(stmt, 1, startDate.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, endDate.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        std::cout << "\nSQL Error: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    return true;
}

bool Database::removePayPeriod(const int& payPeriodID) {
    const char* SQL = "DELETE FROM pay_periods WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_int(stmt, 1, payPeriodID);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        std::cerr << "\nSQL Error: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    if (sqlite3_changes(db) == 0) {
        std::cout << "\nPay Period Not Found\n";
        return false;
    }

    return true;
}

bool Database::insertPayrollForPayPeriod(const int& payPeriodID) {
    // ---------------------------------------------------------- //
    // Validating pay period and retrieving start_date & end_date //
    // ---------------------------------------------------------- //

    int consoleWaitTimer = 500;
    std::cout << "Validating Pay Period ID . . .\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));

    const char* payPeriodSQL = "SELECT start_date, end_date, processed_at FROM pay_periods WHERE id = ?";
    sqlite3_stmt* payPeriodStmt;
    if (sqlite3_prepare_v2(db, payPeriodSQL, -1, &payPeriodStmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_int(payPeriodStmt, 1, payPeriodID);

    int payPeriodResult = sqlite3_step(payPeriodStmt);
    if (payPeriodResult != SQLITE_ROW) {
        std::cerr << "\nPay period not found\n";
        sqlite3_finalize(payPeriodStmt);
        return false;
    }

    const unsigned char* processed = sqlite3_column_text(payPeriodStmt, 2);
    if (processed != nullptr) {
        std::cerr << "Pay period has already been processed\n";
        sqlite3_finalize(payPeriodStmt);
        return false;
    }
    else {
        std::cout << "Pay period processed\n";
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));

    std::string startDate = reinterpret_cast<const char*>(sqlite3_column_text(payPeriodStmt, 0));
    std::string endDate = reinterpret_cast<const char*>(sqlite3_column_text(payPeriodStmt, 1));
    sqlite3_finalize(payPeriodStmt);

    std::cout << "Start Date: " << startDate << '\n' << "End Date: " << endDate << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));

    // ------------------------------------------------------- //
    // Retrieving all active employees for payroll calculation //
    // ------------------------------------------------------- //

    std::cout << "Retrieving active employees for payroll calculation . . ." << '\n';
    std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));

    const char* employeesSQL = "SELECT id, hourly_rate FROM employees WHERE is_active = 1;";
    sqlite3_stmt* employeesStmt;
    if (sqlite3_prepare_v2(db, employeesSQL, -1, &employeesStmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    int employeeCount{ 0 };
    while (sqlite3_step(employeesStmt) == SQLITE_ROW) {
        employeeCount++;
        int employeeID = sqlite3_column_int(employeesStmt, 0);
        double hourlyRate = sqlite3_column_double(employeesStmt, 1);

        std::cout << "Processing Employee ID: " << employeeID << " | Hourly Rate: $" << hourlyRate << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));

        // ------------------------------------------------ //
        // Calculating total hours worked for each employee //
        // ------------------------------------------------ //

        const char* timeEntriesSQL = "SELECT SUM(hours_worked) FROM time_entries WHERE employee_id = ? AND date_worked BETWEEN ? and ?;";
        sqlite3_stmt* timeEntriesStmt;
        if (sqlite3_prepare_v2(db, timeEntriesSQL, -1, &timeEntriesStmt, nullptr) != SQLITE_OK) {
            std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
            sqlite3_finalize(employeesStmt);
            return false;
        }
    
        sqlite3_bind_int(timeEntriesStmt, 1, employeeID);
        sqlite3_bind_text(timeEntriesStmt, 2, startDate.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(timeEntriesStmt, 3, endDate.c_str(), -1, SQLITE_STATIC);

        double totalHours{ 0.0 };
        if (sqlite3_step(timeEntriesStmt) == SQLITE_ROW) {
            totalHours = sqlite3_column_double(timeEntriesStmt, 0);
        }
        sqlite3_finalize(timeEntriesStmt);

        std::cout << "Total Hours Worked: " << totalHours << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));

        // -------------------------------------------------------- //
        // Calculating gross/net pay, and inserting it into payroll // 
        // -------------------------------------------------------- //

        double grossPay = totalHours * hourlyRate;
        double taxRate = 0.15; // 15% tax (you can adjust this or add more taxes)
        double netPay = grossPay * (1 - taxRate); // Deduct taxes here

        std::cout << "Gross Pay: $" << grossPay << " | Net Pay After 15% Tax: $" << netPay << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));
    
        const char* insertPayrollSQL = "INSERT INTO payroll(employee_id, pay_period_id, gross_pay, net_pay) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* insertPayrollStmt;
        if (sqlite3_prepare_v2(db, insertPayrollSQL, -1, &insertPayrollStmt, nullptr) != SQLITE_OK) {
            std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
            sqlite3_finalize(employeesStmt);
            return false;
        }

        sqlite3_bind_int(insertPayrollStmt, 1, employeeID);
        sqlite3_bind_int(insertPayrollStmt, 2, payPeriodID);
        sqlite3_bind_double(insertPayrollStmt, 3, grossPay);
        sqlite3_bind_double(insertPayrollStmt, 4, netPay);

        int insertPayrollResult = sqlite3_step(insertPayrollStmt);
        sqlite3_finalize(insertPayrollStmt);

        if (insertPayrollResult != SQLITE_DONE) {
            std::cerr << "\nFailed to insert payroll: " << sqlite3_errmsg(db) << '\n';
            sqlite3_finalize(employeesStmt);
            return false;
        }

        std::cout << "Payroll Inserted Successfully for Employee ID: " << employeeID << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));
    }

    sqlite3_finalize(employeesStmt);

    if (employeeCount == 0)
        std::cout << "\nNo active employees found for this pay period\n";
    else
        std::cout << "\nPayroll processing completed for " << employeeCount << " employees(s)" << '\n';

    // ------------------------------- //
    // Marking pay period as processed //
    // ------------------------------- //

    const char* updateSQL = "UPDATE pay_periods SET processed_at = CURRENT_TIMESTAMP WHERE id = ?;";
    sqlite3_stmt* updateStmt;
    if (sqlite3_prepare_v2(db, updateSQL, -1, &updateStmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_int(updateStmt, 1, payPeriodID);

    int updateResult = sqlite3_step(updateStmt);
    sqlite3_finalize(updateStmt);

    if (updateResult != SQLITE_DONE) {
        std::cerr << "\nFailed to mark pay period as processed: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(consoleWaitTimer));
    std::cout << "Pay period successfully marked as processed\n";

    return true;
}

bool Database::addTimeEntry(const TimeEntry& timeEntry) {
    const char* SQL = "INSERT INTO time_entries (employee_id, date_worked, hours_worked) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_int(stmt, 1, timeEntry.employeeID);
    sqlite3_bind_text(stmt, 2, timeEntry.dateWorked.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, timeEntry.hoursWorked);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        int errorCode = sqlite3_extended_errcode(db);

        switch (errorCode) {
        case SQLITE_CONSTRAINT_FOREIGNKEY:
            std::cerr << "\nFailed: Employee ID does not exist.\n";
            break;
        case SQLITE_CONSTRAINT_NOTNULL:
            std::cerr << "\nFailed: Missing required value (NULL not allowed).\n";
            break;
        case SQLITE_CONSTRAINT_CHECK:
            std::cerr << "\nFailed: Check constraint violated (e.g., negative hours).\n";
            break;
        case SQLITE_CONSTRAINT_UNIQUE:
            std::cerr << "\nFailed: Duplicate entry where unique value required.\n";
            break;
        default:
            std::cerr << "\nFailed to add time entry: " << sqlite3_errmsg(db) << '\n';
        }

        return false;
    }

    return true;
}

bool Database::setEmployeeStatus(const int& employeeID, const bool& isActive) {
    const char* SQL = "UPDATE employees SET is_active = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_int(stmt, 1, isActive ? 1 : 0);
    sqlite3_bind_int(stmt, 2, employeeID);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        std::cerr << "\nError Setting Employee Status: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    if (sqlite3_changes(db) == 0) {
        std::cout << "\nFailed To Set Employee Status: Please verify employee ID exists and try again\n";
        return false;
    }

    return true;
}

bool Database::addEmployee(const Employee& emp) {
    const char* SQL = "INSERT INTO employees (first_name, last_name, department, employment_type, hourly_rate, hire_date, is_active) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_text(stmt, 1, emp.firstName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, emp.lastName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, emp.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, emp.employmentType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 5, emp.hourlyRate);
    sqlite3_bind_text(stmt, 6, emp.hireDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, emp.isActive ? 1 : 0);

    int result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        std::cerr << "\nFailed To Add Employee, Error: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    return true;
}

std::vector<PayPeriod> Database::getPayPeriods() {
    std::vector<PayPeriod> stored_pay_periods;

    const char* SQL = "SELECT id, start_date, end_date, processed_at FROM pay_periods ORDER BY start_date ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return stored_pay_periods;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PayPeriod payPeriod;

        payPeriod.id = sqlite3_column_int(stmt, 0);
        payPeriod.startDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        payPeriod.endDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        payPeriod.processedAt = sqlite3_column_text(stmt, 3) ? reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";

        stored_pay_periods.push_back(payPeriod);
    }

    sqlite3_finalize(stmt);
    return stored_pay_periods;
}

std::vector<Payroll> Database::getPayrollRecords() {
    std::vector<Payroll> stored_payroll_records;

    const char* SQL = "SELECT pr.employee_id, pr.pay_period_id, e.first_name, e.last_name, "
        "p.start_date, p.end_date, pr.gross_pay, pr.net_pay "
        "FROM payroll pr "
        "JOIN employees e ON pr.employee_id = e.id "
        "JOIN pay_periods p ON pr.pay_period_id = p.id "
        "ORDER BY p.start_date ASC, e.id ASC;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return stored_payroll_records;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Payroll payrollRecord;

        payrollRecord.employeeID = sqlite3_column_int(stmt, 0);
        payrollRecord.payPeriodID = sqlite3_column_int(stmt, 1);
        payrollRecord.firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        payrollRecord.lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        payrollRecord.startDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        payrollRecord.endDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        payrollRecord.grossPay = sqlite3_column_double(stmt, 6);
        payrollRecord.netPay = sqlite3_column_double(stmt, 7);

        stored_payroll_records.push_back(payrollRecord);
    }

    sqlite3_finalize(stmt);
    return stored_payroll_records;
}

std::vector<EmployeePayroll> Database::getEmployeePayrollHistory(const int& employeeID) {
    std::vector<EmployeePayroll> stored_payrolls;

    const char* SQL = "SELECT pr.pay_period_id, "
        "p.start_date, p.end_date, p.processed_at, "
        "pr.gross_pay, pr.net_pay FROM payroll pr "
        "JOIN pay_periods p ON pr.pay_period_id = p.id "
        "WHERE pr.employee_id = ? ORDER BY p.start_date ASC;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return stored_payrolls;
    }

    sqlite3_bind_int(stmt, 1, employeeID);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        EmployeePayroll payroll;

        payroll.payPeriodID = sqlite3_column_int(stmt, 0);
        payroll.startDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        payroll.endDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        payroll.processedAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        payroll.grossPay = sqlite3_column_double(stmt, 4);
        payroll.netPay = sqlite3_column_double(stmt, 5);

        stored_payrolls.push_back(payroll);
    }

    sqlite3_finalize(stmt);
    return stored_payrolls;
}

std::vector<TimeEntry> Database::getTimeEntries() {
    std::vector<TimeEntry> stored_time_entries;

    const char* SQL = "SELECT employee_id, date_worked, hours_worked FROM time_entries ORDER BY date_worked ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return stored_time_entries;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TimeEntry timeEntryRecord;

        timeEntryRecord.employeeID = sqlite3_column_int(stmt, 0);
        timeEntryRecord.dateWorked = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        timeEntryRecord.hoursWorked = sqlite3_column_double(stmt, 2);

        stored_time_entries.push_back(timeEntryRecord);
    }

    sqlite3_finalize(stmt);
    return stored_time_entries;
}

std::vector<Employee> Database::getEmployees() {
    std::vector<Employee> stored_employees;

    const char* SQL = "SELECT id, first_name, last_name, department, employment_type, hire_date, hourly_rate, is_active "
        "FROM employees ORDER BY last_name DESC, first_name DESC;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "\nError sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
        return stored_employees;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Employee emp;

        emp.id = sqlite3_column_int(stmt, 0);
        emp.firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        emp.lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        emp.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        emp.employmentType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        emp.hireDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        emp.hourlyRate = sqlite3_column_double(stmt, 6);
        emp.isActive = sqlite3_column_int(stmt, 7);

        stored_employees.push_back(emp);
    }

    sqlite3_finalize(stmt);
    return stored_employees;
}
