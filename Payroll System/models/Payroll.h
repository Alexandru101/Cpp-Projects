#pragma once

#include <string>

struct Payroll {
	int employeeID;
	int payPeriodID;

	std::string firstName;
	std::string lastName;
	std::string startDate;
	std::string endDate;

	double grossPay;
	double netPay;
};
