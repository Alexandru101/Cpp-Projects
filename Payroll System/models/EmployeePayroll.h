#pragma once

#include <string>

struct EmployeePayroll {
    int payPeriodID;

    std::string startDate;
    std::string endDate;
    std::string processedAt;

    double grossPay;
    double netPay;
};
