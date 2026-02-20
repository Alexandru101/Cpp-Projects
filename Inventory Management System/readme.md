# Inventory-Management-System Documentation

![License](https://github.com/maxcomuk/Cpp-Projects/blob/main/LICENSE) <br></br>
[Direct Exe Download](https://limewire.com/d/9aydG#i1zSbooxiD)

## Project Overview
1. Inventory Management System allows the (user / admin) control the Inventory of a buisness or warehouse etc. Our goal is to use oop and create a class with the required methods to control the inventory and get data aswell as a seperate class that will handle the registration for the user to login into the inventory management system. As we are focusing primarly on the inventory class we will only include simple methods for the registration such as registering an account or logging into an existing account.

2. We will be using sqlite3 to create the database file meaning for the project to work you MUST add sqlite3.c and sqlite3.h to the 'db' folder for it to work and provide the functionalities required for the database. You can do this by downloading the sqlite3.c and .h from this github or from the offical SQL website and then right-click on the files and select the option 'include in project' once this is done on each file they will be added to the project and allow the database to use the sqlite3 methods.

### Key Features
- Inventory Registration
- Inventory Management
- Logout / Exit program compatibility

### Things to know
As this is my first time building an inventory management system and using multiple files, I will be including extra code such as declaring the constructor or deconstructor but it may not be used so you can delete these parts as it was intended for users that may build on this project. Its also recommended to rename the files to your likings such as 'db' can be renamed to database.

## Step 1: Database.h
Database class will contain all the methods that will interact with the database and peform actions such as changing or getting data. The 'InventoryItem' struct will be used as a template when we need to modify an item by using this struct we can pass the desired inventory item data and find it within the database and then change it to the new data, Note we use this struct to pass data through a function overall the benefits of doing this is that we prevent 7+ arguments to function paremeter and just include 1 argument which is the struct itself.

```
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
```

## Step 2: Database.cpp
- Constructor & Destructor
The constructor initializes the db pointer to nullptr, indicating that no database connection is active. The destructor ensures proper resource management. If the database was opened (db is not nullptr), it safely closes the connection using sqlite3_close(db) and resets the pointer to nullptr. This guarantees that the database connection is properly released when the Database object goes out of scope, preventing resource leaks.

2. OpenDatabase
The OpenDatabase method takes a fileName parameter and attempts to open (or create, if it does not exist) a SQLite database file using sqlite3_open(). If the operation fails, it prints the error message, closes any partially opened connection, resets the pointer, and returns false. If successful, the db pointer now represents a valid database connection and the method returns true.

3. CreateTable
The CreateTable method dynamically builds a SQL query using the provided tableName and columns parameters. It executes a CREATE TABLE IF NOT EXISTS statement to ensure the table is created only if it does not already exist.

In this project, it is used to create:

USERS – stores registered users for authentication.
INVENTORY – stores all inventory-related data for the management system.

The method returns true if the table is successfully created, otherwise false.

4. ValidateUser
This method verifies whether a user exists in the USERS table by matching the provided username and password. It uses a prepared statement with parameter binding to prevent SQL injection. The query counts matching rows. If the count is greater than 0 → returns true. Otherwise → returns false. This method is used during the login process.

5. InsertUser
The InsertUser method inserts a new user record into the USERS table using parameter binding. Returns true if insertion is successful (SQLITE_DONE). Returns false if the operation fails (e.g., duplicate user constraint or other database error). This method supports user registration.

6. RemoveUser
This method removes a user from the USERS table by matching both username and password. After execution, it checks sqlite3_changes(db) to confirm that a row was actually deleted. Returns true if a user was removed. Returns false if no matching user exists or an error occurs.

7. ValidateItem
The ValidateItem method checks whether an inventory item exists in the INVENTORY table using the productName field from the InventoryItem struct.

If a matching row is found:
The struct is populated with all retrieved database values.
Default fallback values are assigned if any column is NULL.
Returns true.

If no record is found, it returns false.

8. ModifyItem
This method updates an existing inventory item. It locates the item using originalProductName and updates all relevant fields using the new values provided in the InventoryItem struct. The CREATED_AT column is updated automatically using CURRENT_TIMESTAMP.

After execution:
If at least one row was modified → returns true.
If no matching product exists or an error occurs → returns false.

9. InsertItem
The InsertItem method inserts a new record into the INVENTORY table using the data stored in the InventoryItem struct.

Returns true if the insertion succeeds.
Returns false if the product already exists (constraint failure) or another database error occurs.

This method is used to add new products to the inventory system.

10. RemoveItem
This method deletes an item from the INVENTORY table using the provided productName.

After execution, it verifies that a row was deleted by checking sqlite3_changes(db).
Returns true if the product was successfully removed.
Returns false if the product does not exist or if a database error occurs.

11. GetItem
The GetItem method retrieves a specific inventory item by matching both category and productName from the provided InventoryItem struct.

If a matching row is found:
The remaining fields (amount, price, size, description, createdAt) are populated from the database.
Returns true.
If no match is found, it prints a message and returns false.

This method is useful when searching for a specific product within a known category.

12. GetAllItems
The GetAllItems method retrieves all records from the INVENTORY table.

Process overview:
A SELECT query is prepared to fetch every column from the table.
The method iterates through each row using sqlite3_step() inside a while loop.

For each row:
A new InventoryItem object is created.
All database fields are extracted and assigned to the object.
Default fallback values are used if any field is NULL.
The fully populated object is pushed into the storedItems vector.
After all rows are processed:
The prepared statement is finalized.
The method returns a std::vector<InventoryItem> containing every item in the inventory database.

This method is essential for displaying or processing the entire inventory list in the application.
```
#include <iostream>
#include "Database.h"

Database::Database() : db(nullptr) {}

Database::~Database()
{
	if (db)
	{
		sqlite3_close(db);
		db = nullptr;
	}
}

bool Database::OpenDatabase(const std::string& fileName)
{
	int response = sqlite3_open(fileName.c_str(), &db);
	if (response != SQLITE_OK)
	{
		std::cerr << "\nFailed to open database: " << sqlite3_errmsg(db) << '\n';

		if (db)
		{
			sqlite3_close(db);
			db = nullptr;
		}

		return false;
	}

	return true;
}

bool Database::CreateTable(const std::string& tableName, const std::string& columns)
{
	std::string SQL = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + columns + ");";

	int response = sqlite3_exec(db, SQL.c_str(), nullptr, nullptr, nullptr);
	if (response != SQLITE_OK)
		std::cerr << "\nFailed to create database table " << tableName << ": " << sqlite3_errmsg(db) << '\n';
	else
		return true;

	return false;
}

bool Database::ValidateUser(const std::string& username, const std::string& password)
{
	const char* SQL = "SELECT count(*) FROM USERS WHERE USERNAME = ? AND PASSWORD = ?";
	sqlite3_stmt* validate_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &validate_stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(validate_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(validate_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	bool userValidated = false;
	if (sqlite3_step(validate_stmt) == SQLITE_ROW)
	{
		if (sqlite3_column_int(validate_stmt, 0) > 0)
		{
			userValidated = true;
		}
	}

	sqlite3_finalize(validate_stmt);
	return userValidated;
}

bool Database::InsertUser(const std::string& username, const std::string& password)
{
	const char* SQL = "INSERT INTO USERS (USERNAME, PASSWORD) VALUES (?, ?);";
	sqlite3_stmt* insert_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &insert_stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(insert_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(insert_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	int result = sqlite3_step(insert_stmt);
	sqlite3_finalize(insert_stmt);

	if (result == SQLITE_DONE)
		return true;

	if (result == SQLITE_CONSTRAINT)
		std::cerr << "\nFailed to register user: Account arleady exists!\n";
	else
		std::cerr << "\nFailed to register user: " << sqlite3_errmsg(db) << '\n';

	return false;
}

bool Database::RemoveUser(const std::string& username, const std::string& password)
{
	const char* SQL = "DELETE FROM USERS WHERE USERNAME = ? AND PASSWORD = ?;";
	sqlite3_stmt* delete_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &delete_stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(delete_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(delete_stmt, 2, password.c_str(), -1, SQLITE_STATIC);

	int result = sqlite3_step(delete_stmt);
	sqlite3_finalize(delete_stmt);

	if (result == SQLITE_DONE)
	{
		if (sqlite3_changes(db) > 0)
			return true;

		std::cerr << "\nUser Not Found: Please try again!\n";
		return false;
	}

	std::cerr << "\nDatabase Error During Removing User: " << sqlite3_errmsg(db) << '\n';
	return false;
}

bool Database::ValidateItem(InventoryItem& object)
{
	const char* SQL = "SELECT CATEGORY, PRODUCT_NAME, AMOUNT, PRICE, SIZE, DESCRIPTION, CREATED_AT FROM INVENTORY WHERE PRODUCT_NAME = ?;";
	sqlite3_stmt* validate_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &validate_stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(validate_stmt, 1, object.productName.c_str(), -1, SQLITE_STATIC);

	bool productValidated = false;
	if (sqlite3_step(validate_stmt) == SQLITE_ROW)
	{
		const char* cat = (const char*)sqlite3_column_text(validate_stmt, 0);
		const char* name = (const char*)sqlite3_column_text(validate_stmt, 1);
		const char* size = (const char*)sqlite3_column_text(validate_stmt, 4);
		const char* desc = (const char*)sqlite3_column_text(validate_stmt, 5);
		const char* date = (const char*)sqlite3_column_text(validate_stmt, 6);

		object.category = cat ? cat : "Unknown";
		object.productName = name ? name : "Unknown";
		object.amount = sqlite3_column_int(validate_stmt, 2);
		object.price = sqlite3_column_double(validate_stmt, 3);
		object.size = size ? size : "0kg";
		object.description = desc ? desc : "No Description";
		object.createdAt = date ? date : "N/A";

		productValidated = true;
	}

	sqlite3_finalize(validate_stmt);
	return productValidated;
}

bool Database::ModifyItem(const std::string& originalProductName, const InventoryItem& object)
{
	const char* SQL = "UPDATE INVENTORY SET CATEGORY = ?, PRODUCT_NAME = ?, AMOUNT = ?, PRICE = ?, SIZE = ?, DESCRIPTION = ?, CREATED_AT = CURRENT_TIMESTAMP WHERE PRODUCT_NAME = ?;";
	sqlite3_stmt* update_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &update_stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(update_stmt, 1, object.category.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(update_stmt, 2, object.productName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(update_stmt, 3, object.amount);
	sqlite3_bind_double(update_stmt, 4, object.price);
	sqlite3_bind_text(update_stmt, 5, object.size.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(update_stmt, 6, object.description.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(update_stmt, 7, originalProductName.c_str(), -1, SQLITE_STATIC);

	int result = sqlite3_step(update_stmt);
	sqlite3_finalize(update_stmt);

	if (result == SQLITE_DONE)
	{
		if (sqlite3_changes(db) > 0)
			return true;

		std::cerr << "\nUpdate Failed: Product " << originalProductName << " Not Found!\n";
		return false;
	}

	std::cerr << "\nUpdate Failed: Database Error -> " << sqlite3_errmsg(db) << '\n';
	return false;
}

bool Database::InsertItem(const InventoryItem& object)
{
	const char* SQL = "INSERT INTO INVENTORY (CATEGORY, PRODUCT_NAME, AMOUNT, PRICE, SIZE, DESCRIPTION) VALUES (?, ?, ?, ?, ?, ?);";
	sqlite3_stmt* insert_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &insert_stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_preprare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(insert_stmt, 1, object.category.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(insert_stmt, 2, object.productName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(insert_stmt, 3, object.amount);
	sqlite3_bind_double(insert_stmt, 4, object.price);
	sqlite3_bind_text(insert_stmt, 5, object.size.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(insert_stmt, 6, object.description.c_str(), -1, SQLITE_STATIC);

	int result = sqlite3_step(insert_stmt);
	sqlite3_finalize(insert_stmt);

	if (result == SQLITE_DONE)
		return true;

	if (result == SQLITE_CONSTRAINT)
		std::cerr << "\nFailed to add product to inventory: Product arleady exists!\n";
	else
		std::cerr << "\nFailed to add product to inventory: " << sqlite3_errmsg(db) << '\n';

	return false;
}

bool Database::RemoveItem(const std::string& productName)
{
	const char* SQL = "DELETE FROM INVENTORY WHERE PRODUCT_NAME = ?;";
	sqlite3_stmt* delete_stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &delete_stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(delete_stmt, 1, productName.c_str(), -1, SQLITE_STATIC);

	int result = sqlite3_step(delete_stmt);
	sqlite3_finalize(delete_stmt);

	if (result == SQLITE_DONE)
	{
		if (sqlite3_changes(db) > 0)
			return true;

		std::cerr << "\nFailed to remove product from the inventory. Please try again!\n";
		return false;
	}

	std::cerr << "\nDatabase Error During Removing Product: " << sqlite3_errmsg(db) << '\n';
	return false;
}

bool Database::GetItem(InventoryItem& object)
{
	const char* SQL = "SELECT CATEGORY, PRODUCT_NAME, AMOUNT, PRICE, SIZE, DESCRIPTION, CREATED_AT FROM INVENTORY WHERE CATEGORY = ? AND PRODUCT_NAME = ?;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) != SQLITE_OK)
	{
		std::cerr << "\nError Preparing sqlite3_prepare_v2: " << sqlite3_errmsg(db) << '\n';
		return false;
	}

	sqlite3_bind_text(stmt, 1, object.category.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, object.productName.c_str(), -1, SQLITE_STATIC);

	int result = sqlite3_step(stmt);
	bool exists = false;

	if (result == SQLITE_ROW)
	{
		const char* size = (const char*)sqlite3_column_text(stmt, 4);
		const char* desc = (const char*)sqlite3_column_text(stmt, 5);
		const char* date = (const char*)sqlite3_column_text(stmt, 6);

		object.amount = sqlite3_column_int(stmt, 2);
		object.price = sqlite3_column_double(stmt, 3);
		object.size = size ? size : "0kg";
		object.description = desc ? desc : "No Description";
		object.createdAt = date ? date : "N/A";

		exists = true;
	}
	else
	{
		std::cout << "\nNo item found with name: " << object.productName << '\n';
	}

	sqlite3_finalize(stmt);
	return exists;
}

std::vector<InventoryItem> Database::GetAllItems()
{
	std::vector<InventoryItem> storedItems;

	const char* SQL = "SELECT CATEGORY, PRODUCT_NAME, AMOUNT, PRICE, SIZE, DESCRIPTION, CREATED_AT FROM INVENTORY;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db, SQL, -1, &stmt, nullptr) == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			InventoryItem item;

			const char* cat = (const char*)sqlite3_column_text(stmt, 0);
			const char* name = (const char*)sqlite3_column_text(stmt, 1);
			const char* size = (const char*)sqlite3_column_text(stmt, 4);
			const char* desc = (const char*)sqlite3_column_text(stmt, 5);
			const char* date = (const char*)sqlite3_column_text(stmt, 6);

			item.category = cat ? cat : "Unknown";
			item.productName = name ? name : "Unknown";
			item.amount = sqlite3_column_int(stmt, 2);
			item.price = sqlite3_column_double(stmt, 3);
			item.size = size ? size : "0kg";
			item.description = desc ? desc : "No Description";
			item.createdAt = date ? date : "N/A";

			storedItems.push_back(item);
		}
	}
	else {
		std::cerr << "\nFailed to fetch items: " << sqlite3_errmsg(db) << '\n';
	}

	sqlite3_finalize(stmt);
	return storedItems;
}
```
