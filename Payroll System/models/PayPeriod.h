#pragma once

#include <string>

struct PayPeriod {
	int id;
	std::string startDate;
	std::string endDate;
	std::string processedAt;

	bool isProcessed() const {
		return !processedAt.empty();
	}
};
