// Including our header file with the essential requirements
#include "Main.h"

// Manages authentication logic for the user registration and login that will control the database management system (this is essentially the admin)
class AuthManager
{
private:
	// Private members to prevent tampering outside of the class
	std::string username;
	std::string password;

	// Signal that will determine whether the user is logged in
	bool userLoggedIn = false;
public:
	// Logging in the user into the database if his credentials are valid
	bool logginUser(sqlite3*& db)
	{
		// SQL script that will attempt to find if the username and password received from the user is valid
		const char* loginSQL = "SELECT count(*) FROM USERS WHERE USERNAME = ? AND PASSWORD = ?;";
		sqlite3_stmt* login_stmt;

		// Grabbing user input
		std::cout << "\nUsername: ";
		std::getline(std::cin, username);

		std::cout << "Password: ";
		std::getline(std::cin, password);

		// Conditional if statement to validate if the sqlite3_prepare_v2 was successful
		if (sqlite3_prepare_v2(db, loginSQL, -1, &login_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to prepare sqlite3: " << sqlite3_errmsg(db) << '\n';
			return false;
		}

		// Binding both (USERNAME = ? and PASSWORD = ?) question marks with the users input (username and password)
		// This is important to prevent basic sql injection attacks
		sqlite3_bind_text(login_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(login_stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

		// Checking if the user exists and credentials are valid
		int count = 0;
		if (sqlite3_step(login_stmt) == SQLITE_ROW)
		{
			count = sqlite3_column_int(login_stmt, 0);
		}

		// Finalizing the prepared statement and releasing its resources
		sqlite3_finalize(login_stmt);

		if (count == 0)
		{
			std::cout << "\nInvalid Credentials: Please try again\n";
			return false;
		}
		else
		{
			std::cout << "\nLogged Into Account Successfully\n";
			userLoggedIn = true;
			return true;
		}

		return false;
	}

	// Registering a user to the database if it doesn't already exist
	void registerUser(sqlite3*& db)
	{
		// SQL script that will add into the users table from the database the users registration strings such as USERNAME and PASSWORD
		const char* saveSQL = "INSERT INTO USERS (USERNAME, PASSWORD) VALUES (?, ?);";
		sqlite3_stmt* save_stmt;

		// Grabbing users registration input
		std::cout << "\nUsername: ";
		std::getline(std::cin, username);

		std::cout << "Password: ";
		std::getline(std::cin, password);

		// Checking if the sqlite3_prepare_v2 was successful
		if (sqlite3_prepare_v2(db, saveSQL, -1, &save_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to prepare sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Binding both (USERNAME = ? and PASSWORD = ?) question marks with the users input
		// This is important to prevent basic SQL injection
		sqlite3_bind_text(save_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(save_stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);

		/* Executes the prepared statement and handles the result: success or UNIQUE constraint violation which means
		   the account already exists or its a database error */
		int response = sqlite3_step(save_stmt);
		if (response == SQLITE_DONE)
		{
			std::cout << "\nAccount Registered Successfully\n";
		}
		else if (response == SQLITE_CONSTRAINT)
		{
			std::cout << "\nError: Account Arleady Exists\n";
		}
		else {
			std::cerr << "Error: " << sqlite3_errmsg(db) << '\n';
		}

		// Finalizing the prepared statement and releasing its resources
		sqlite3_finalize(save_stmt);
	}

	// Public method of the class that will set the userLoggedIn variable to false (we are specifying the user has logged out of the account)
	void LogOut() { userLoggedIn = false; };
};

// Setting up user registration for the database, this will be the login required for the user that is controlling the database eg admin
void setup_user_registration(AuthManager& user, sqlite3*& db)
{
	// Placeholder for the users decision (logging in, registering account or exiting program)
	int decision;

	// Loop that will keep the registration alive until we decide to exit or have created / logged into an account
	while (true)
	{
		// Prompting menu navigation (decisions)
		showMenu();
		if (!(std::cin >> decision && (decision > 0 && decision < 4)))
		{
			std::cout << "Invalid Input: Please try again\n";
			ClearInputBuffer();
			continue;
		}
		
		// Clearing input buffer (stream) and leftover whitespaces
		ClearInputBuffer();

		// Conditional if statements that will either prompt user loggin registration or account registration or exiting program
		if (decision == 1)
		{
			if (user.logginUser(db))
				break;
		}
		else if (decision == 2)
		{
			user.registerUser(db);
		}
		else
		{
			std::cout << "\nClosing Program . . .\n";
			sqlite3_close(db); // Important to close the database before exiting program completly
			exit(0); // number 0 signals the program exited successfully but this is optional
		}
	}
}

// Student database management system (this will manage all the database features)
struct schoolHandler
{
	// Searching for the student inside the database
	void searchUser(sqlite3*& db)
	{
		std::string userInput, username, country, religion;
		int age;

		std::cout << "Username: ";
		std::getline(std::cin, userInput);

		// SQL query to retrieve student details by name
		const char* checkSQL = "SELECT NAME, AGE, PLACE_OF_BIRTH, RELIGION FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* check_stmt;

		// Conditional if statement that will prepare the sqlite3_prepare_v2 and check if it was successful 
		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Binding (? from NAME = ?) question mark with the users input. Note we use ? instead of NAME = username so that we can prevent basic SQL attacks
		sqlite3_bind_text(check_stmt, 1, userInput.c_str(), -1, SQLITE_TRANSIENT);

		// Conditional if statement that will execute the query and display student information if student exists
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			std::cout << "\n**Student Details**\n";
			std::cout << "Name: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Age: " << sqlite3_column_int(check_stmt, 1) << '\n';
			std::cout << "Place Of Birth: " << sqlite3_column_text(check_stmt, 2) << '\n';
			std::cout << "Religion: " << sqlite3_column_text(check_stmt, 3) << '\n';
		}
		else
		{
			std::cout << "\nStudent Not Found!\n";
		}

		// Finalising prepared statement and releasing its resources 
		sqlite3_finalize(check_stmt);
	}

	// Adding student to the database (this will be inside the STUDENTS table created on the database)
	void addStudent(sqlite3*& db)
	{
		std::string username, country, religion;
		int age;

		std::cout << "Name: ";
		std::getline(std::cin, username);

		std::cout << "Age: ";
		if (!(std::cin >> age))
		{
			std::cout << "\nInvalid Input: Please try again\n";
			ClearInputBuffer();
			return;
		}

		ClearInputBuffer();

		std::cout << "Place of birth: ";
		std::getline(std::cin, country);

		std::cout << "Religion: ";
		std::getline(std::cin, religion);

		// SQL script that will check if the student exists
		const char* checkSQL = "SELECT count(*) FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db);
			return;
		}

		// Binding ? from NAME = ? with the users input
		sqlite3_bind_text(check_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_step(check_stmt); // Executing query check_stmt

		// Checking if the student already exists then finalising prepared statement and releasing its resources 
		int count = sqlite3_column_int(check_stmt, 0);
		sqlite3_finalize(check_stmt);

		if (count > 0)
		{
			std::cout << "\nInvalid Username: Student Arleady Exists\n";
			return;
		}

		// SQL script that will add the student alongside with his details to the database
		const char* saveSQL = "INSERT INTO STUDENTS (NAME, AGE, PLACE_OF_BIRTH, RELIGION) VALUES (?, ?, ?, ?);";
		sqlite3_stmt* save_stmt;

		if (sqlite3_prepare_v2(db, saveSQL, -1, &save_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db);
			return;
		}

		// Binding all four (?) with the users input to the SQL script that will insert the values to the database
		sqlite3_bind_text(save_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(save_stmt, 2, age);
		sqlite3_bind_text(save_stmt, 3, country.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_text(save_stmt, 4, religion.c_str(), -1, SQLITE_TRANSIENT);

		// Conditional if statement that will show cause of error if the save_stmt failed to insert the student to the database
		int response = sqlite3_step(save_stmt);
		if (response != SQLITE_DONE)
			std::cerr << "\nError: " << sqlite3_errmsg(db) << '\n';
		else
			std::cout << "\nStudent Registered Successfully\n";

		sqlite3_finalize(save_stmt);
	}

	// Removing student from the database table named "STUDENTS"
	void removeStudent(sqlite3*& db)
	{
		std::string username;

		std::cout << "Username: ";
		std::getline(std::cin, username);

		// SQL script that will check if the userinput (username) matches any students withing the database
		const char* checkSQL = "SELECT count(*) FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Binding (?) inside the SQL script with the users input so we can validate if the student exists inside the database
		sqlite3_bind_text(check_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

		int count = 0;
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			count = sqlite3_column_int(check_stmt, 0);
		}
		sqlite3_finalize(check_stmt);

		if (count == 0)
		{
			std::cout << "\nStudent Not Found!\n";
			return;
		}

		// SQL script that will delete the student from the database alongside with all the details tied to that student
		const char* deleteSQL = "DELETE FROM STUDENTS WHERE NAME = ?;";
		sqlite3_stmt* delete_stmt;

		if (sqlite3_prepare_v2(db, deleteSQL, -1, &delete_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Binding (?) inside the SQL script with the users selected student (we do this to check if the student exists and obviosuly prevent basic SQL attacks)
		sqlite3_bind_text(delete_stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

		// Conditional if statement that will check if the delete_stmt operation was successful otherwise we prompt the cause of error
		if (sqlite3_step(delete_stmt) != SQLITE_DONE)
		{
			std::cout << "\nError Removing Student" << sqlite3_errmsg(db) << '\n';
		}
		else
		{
			std::cout << "\nRemoved Student Successfully\n";
		}

		sqlite3_finalize(delete_stmt);
	}

	// Displaying all registerd students inside the database
	void displayStudents(sqlite3*& db)
	{
		// SQL script that will select a student from the database class "STUDENTS" and all its details tied to that student (AGE, PLACE_OF_BIRTH and RELIGION)
		const char* checkSQL = "SELECT NAME, AGE, PLACE_OF_BIRTH, RELIGION FROM STUDENTS;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		std::cout << "\n**** Students List ****\n";

		bool usersExist = false; // Boolean that will be set to true if we find any registerd students inside the database
		int counter = 0; // Display counter (optional)

		// While loop that will reiterate through the whole STUDENTS database class and print out the details tied to that student
		while (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			usersExist = true;
			counter++;

			std::cout << "STUDENT " << counter << '\n';
			std::cout << "Name: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Age: " << sqlite3_column_int(check_stmt, 1) << '\n';
			std::cout << "Place Of Birth: " << sqlite3_column_text(check_stmt, 2) << '\n';
			std::cout << "Religion: " << sqlite3_column_text(check_stmt, 3) << '\n';
			std::cout << "---------------------------------\n\n";
		}

		// Conditional if statement that will specify to the user, no students found as the boolean "usersExist" is still set to false
		if (!usersExist)
			std::cout << "\nNot Found Users\n";

		sqlite3_finalize(check_stmt);
	}
	
	// Displaying all courses withint the COURSES table inside the database
	void displayCourses(sqlite3*& db)
	{
		// SQL script that will select the course and passgrade tied to it
		const char* checkSQL = "SELECT COURSE, PASSGRADE FROM COURSES;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		std::cout << "\n**** Courses List ****\n";

		bool coursesExist = false; // Boolean that specifies if any course exists inside the COURSES database table
		int counter = 0; // Display counter (optional)

		// While loop that reiterates through the whole COURSES database table and prints out all the details tied to that course
		while (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			coursesExist = true;
			counter++;

			std::cout << "COURSE " << counter << '\n';
			std::cout << "Subject: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Pass Grade: " << sqlite3_column_int(check_stmt, 1) << '\n';
			std::cout << "---------------------------------\n\n";
		}

		// Conditional if statement that will print to the user no course was found if boolean remains false
		if (!coursesExist)
			std::cout << "\nNot Found Courses\n";

		sqlite3_finalize(check_stmt);
	}

	// Adding course to the COURSES database table
	void addCourse(sqlite3*& db)
	{
		std::string courseName;
		int passGrade;

		std::cout << "Subject: ";
		std::getline(std::cin, courseName);

		std::cout << "Pass Grade: ";
		if (!(std::cin >> passGrade))
		{
			ClearInputBuffer();

			std::cout << "\nInvalid Input: Please try again\n";
			return;
		}

		ClearInputBuffer(); // Important we clear the input buffer (stream) and any leftover whitespaces

		// SQL script that will select the course from COURSES
		const char* checkSQL = "SELECT count(*) FROM COURSES WHERE COURSE = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Binding (?) inside of check_stmt SQL query with the users input (courseName) so we can validate if the course exist and is valid
		
		/* Note I havn't explained yet but if we dont bind the (?) we are essentially checking the course with a uknown value (?) meaning
		   we will never find nor check the course with the users input as the (?) is a blank operation that is meant to be asigned through
		   this bind text function that is offerd by the sqlite3 library*/

		sqlite3_bind_text(check_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);

		// Checking if the course exists
		int count = 0;
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			count = sqlite3_column_int(check_stmt, 0);
		}

		sqlite3_finalize(check_stmt);

		if (count > 0)
		{
			std::cout << "\nCourse Already Exists!\n";
			return; // Early exiting to prevent the user from adding two courses with the same name
		}

		// SQL script that will in insert into the COURSES table inside database the course (courseName) and passgrade
		const char* saveSQL = "INSERT INTO COURSES (COURSE, PASSGRADE) VALUES (?, ?);";
		sqlite3_stmt* save_stmt;

		if (sqlite3_prepare_v2(db, saveSQL, -1, &save_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Again important we bind both (?) with the users input
		sqlite3_bind_text(save_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(save_stmt, 2, passGrade);

		// Conditional if statement that will execute the save_stmt query and check if the operation was successful
		int response = sqlite3_step(save_stmt);
		if (response != SQLITE_DONE)
			std::cout << "\nFailed To Save Course: " << sqlite3_errmsg(db) << '\n';
		else
			std::cout << "\nCourse Saved Successfully\n";

		sqlite3_finalize(save_stmt); // Remmeber important to finalize prepared statement and release resources (eg freeing memory)
	}

	// Removing course from the COURSES table inside the database
	void removeCourse(sqlite3*& db)
	{
		std::string courseName;
		int passGrade;

		std::cout << "Course Name: ";
		std::getline(std::cin, courseName);

		std::cout << "Pass Grade: ";
		if (!(std::cin >> passGrade))
		{
			ClearInputBuffer();

			std::cout << "\nInvalid Input: Please try again\n";
			return;
		}

		ClearInputBuffer();

		// SQL script that will select the course from COURSES table inside the database
		const char* checkSQL = "SELECT count(*) FROM COURSES WHERE COURSE = ? AND PASSGRADE = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Important we bind the (?) with the users input
		sqlite3_bind_text(check_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(check_stmt, 2, passGrade);

		// Checking if the course exists for us to delete it later otherwise we early exit here
		int count = 0;
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
			count = sqlite3_column_int(check_stmt, 0);
		sqlite3_finalize(check_stmt);

		if (count == 0)
		{
			std::cout << "\nCourse Not Found: Please try again\n";
			return;
		}

		// SQL script that will delete the course from COURSES table inside the database
		const char* deleteSQL = "DELETE FROM COURSES WHERE COURSE = ?; AND PASSGRADE = ?;";
		sqlite3_stmt* delete_stmt;

		// Remmeber we are peforming a conditional if statement to check if the sqlite3_prepare_v2 was successful otherwise we early exit
		if (sqlite3_prepare_v2(db, deleteSQL, -1, &delete_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db) << '\n';
			return;
		}

		// Binding (?) to the users input
		sqlite3_bind_text(delete_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(delete_stmt, 2, passGrade);

		// Conditional if statement that will print to the user if the operation was successful (this is useful for debugging and checking if the course was removed)
		if (sqlite3_step(delete_stmt) != SQLITE_DONE)
			std::cerr << "\nFailed To Delete Course: " << sqlite3_errmsg(db) << '\n';
		else
			std::cout << "\nCourse Removed Successfully\n";

		sqlite3_finalize(delete_stmt);
	}

	// Searing course from the COURSES table inside the database
	void searchCourse(sqlite3*& db)
	{
		std::string courseName;
		int passGrade;

		std::cout << "Course Name: ";
		std::getline(std::cin, courseName);

		std::cout << "Pass Grade: ";
		if (!(std::cin >> passGrade))
		{
			ClearInputBuffer();

			std::cout << "\nInvalid Input: Please try again\n";
			return;
		}

		ClearInputBuffer();

		// SQL script that will select the course from the COURSES table inside the database
		const char* checkSQL = "SELECT COURSE, PASSGRADE FROM COURSES WHERE COURSE = ? AND PASSGRADE = ?;";
		sqlite3_stmt* check_stmt;

		if (sqlite3_prepare_v2(db, checkSQL, -1, &check_stmt, nullptr) != SQLITE_OK)
		{
			std::cerr << "\nFailed to load sqlite3: " << sqlite3_errmsg(db);
			return;
		}

		// Binding (?) to the users input
		sqlite3_bind_text(check_stmt, 1, courseName.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_bind_int(check_stmt, 2, passGrade);

		// Conditional if statement that will print out the course details if it finds the specified name
		if (sqlite3_step(check_stmt) == SQLITE_ROW)
		{
			std::cout << "\nSubject: " << sqlite3_column_text(check_stmt, 0) << '\n';
			std::cout << "Pass Grade: " << sqlite3_column_int(check_stmt, 1) << '\n';
		}
		else
		{
			std::cout << "\nNot Found Course!\n";
		}

		sqlite3_finalize(check_stmt);
	}
};

// Setting up the database navigation menu (this is the database management system and will handle all logic for the database)
void setup_database_management(schoolHandler& schoolSystem, AuthManager& user, sqlite3*& db)
{
	int decision; // Integer that will store the users selected option

	// While loop that will keep the program alive until the user has decided to exit or logout
	while (true)
	{
		showDatabaseMenu(); // Showing database navigation menu (shows all database options for the user)
		if (!(std::cin >> decision && (decision > 0 && decision < 11)))
		{
			std::cout << "Invalid Input: Please try again\n";
			ClearInputBuffer();
			continue;
		}

		ClearInputBuffer(); // Clearing input stream and any leftover whitespaces
		system("cls"); // Clearing the console screen (this is completly optional, I do this to make the console look organised)

		// Peforming multiple conditional if statements that will execute the function tied to the users decision
		if (decision == 1)
		{
			schoolSystem.searchUser(db);
		}
		else if (decision == 2)
		{
			schoolSystem.addStudent(db);
		}
		else if (decision == 3)
		{
			schoolSystem.removeStudent(db);
		}
		else if (decision == 4)
		{
			schoolSystem.displayStudents(db);
		}
		else if (decision == 5)
		{
			schoolSystem.displayCourses(db);
		}
		else if (decision == 6)
		{
			schoolSystem.addCourse(db);
		}
		else if (decision == 7)
		{
			schoolSystem.removeCourse(db);
		}
		else if (decision == 8)
		{
			schoolSystem.searchCourse(db);
		}
		else if (decision == 9)
		{
			user.LogOut();
			break;
		}
		else if (decision == 10)
		{
			std::cout << "Closing Program . . .\n";
			sqlite3_close(db); // Important to close database before exiting program
			exit(0);
		}

		// Halting the user until he presses "ENTER" so that we dont skip any error messages and can reiterate the loop cleanly
		std::cout << "\nPress Enter To Continue";
		std::cin.get();
	}
}

// Program entry point
int main()
{
	// Initialzing the authmanager and schoolhandler class
	AuthManager user;
	schoolHandler schoolSystem;
	
	// Displaying program information (optional)
	std::cout << "**** Student Database Manager ****\n";
	
	// (Initializing / Creating) the database
	sqlite3* DataBase = nullptr;
	setupDatabase(DataBase);

	// Main program loop that will keep it running
	while (true)
	{
		setup_user_registration(user, DataBase);
		setup_database_management(schoolSystem, user, DataBase);
		system("cls"); // Clearing console. This is completly optional but is prefferd as it cleans the console of any input therefore making it more organised
	}

	/* Although we dont break out the while loop and exit manually inside both setup_user_registration and setup_database_management
	   it still doesnt hurt to be extra secure and free any resources (close database) of memory aswell as returning 0 which will signal
	   the program finished successfully. Note if you also want to expand then you may want to have functionailities that will break
	   out of the while loop to handle other logic here */
	
	// Optional cleanup //
	sqlite3_close(DataBase);
	return 0;
}
