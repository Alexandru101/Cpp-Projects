#pragma once

#include <string>
#include <vector>
#include "sqlite3.h"

struct InventoryItem
{
	std::string category;
	std::string productName;
	int amount;
	double price;
	std::string size;
	std::string description;
	std::string createdAt;
};

class Database
{
private:
	sqlite3* db;
public:
	Database();
	~Database();

	bool OpenDatabase(const std::string& fileName);
	bool CreateTable(const std::string& tableName, const std::string& columns);
	bool ValidateUser(const std::string& username, const std::string& password);
	bool InsertUser(const std::string& username, const std::string& password);
	bool RemoveUser(const std::string& username, const std::string& password);
	
	bool ValidateItem(InventoryItem& object);
	bool ModifyItem(const std::string& productName, const InventoryItem& object);
	bool InsertItem(const InventoryItem& object);
	bool RemoveItem(const std::string& productName);

	bool GetItem(InventoryItem& object);
	std::vector<InventoryItem> GetAllItems();
};
