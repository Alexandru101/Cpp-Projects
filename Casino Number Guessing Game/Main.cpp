// Including necessary libraries //
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <string>

// Clearing the input stream and any leftover whitespaces //
void clearInputStream()
{
	// If we receive any errors from std::cin we clear the errors then clear the input stream below with it //
	if (std::cin.fail())
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Creating a strcut that will hold our data variables for the user //
struct gameResults
{
	int difficultyLevel;
	int betAmount;
	bool won;
};

// Creating a class that will contain all the necessary methods we need to play the game //
class UserData
{
private:
	// Privating all our variables so that anybody reading this code or we may forget that we should not tamper with these //
	int difficultyLevel;
	int currentCash;

	const int maxLevel{ 3 };
	const int minLevel{ 1 };

	// Here we create a vector that will contain all our game history once we save data in it later on //
	std::vector <gameResults> history;
public:
	/* Initializing constructors so that if we dont specify any arguments we start with $100 cash however if we decide to config our game to start with
	   more than $100 we can easily do so by providing the amount we want through the parameter */
	UserData() : currentCash{ 100 } {};
	UserData(const int startingCash) : currentCash{ startingCash } {};

	// Prompting user with the difficulty level from 1 to 3 then setting it to their input / decision //
	void set_difficulty_level()
	{
		while (true)
		{
			std::cout << "Choose game difficulty from 1 - 3: ";
			if (std::cin >> difficultyLevel)
			{
				if (difficultyLevel >= minLevel && difficultyLevel <= maxLevel)
					break;
				else
					std::cout << "\nInvalid Number: Please try again" << '\n';
			}
			else
			{
				std::cout << "\nInvalid Number: Please try again" << '\n';
			}

			clearInputStream();
		}
	}

	// Grabbing the difficultyLevel variable from private section then returning a copy //
	int get_difficulty_level() const { return difficultyLevel; };

	// Grabbing the cash variable from private section then returning a copy //
	int get_cash_balance() const { return currentCash; };

	int get_number_of_retries() const
	{
		if (difficultyLevel == 1) { return 3; };
		if (difficultyLevel == 2) { return 2; };
		
		return 1;
	}

	// Saving the results into a new copy of struct then adding it to the game history vector defined in the private section of user class //
	void saveResults(const int& difficulty, const int& amount, const bool& gameWon)
	{
		// instantiating a copy of struct //
		gameResults data;

		// Modifying values to the game save //
		data.difficultyLevel = difficulty;
		data.betAmount = amount;
		data.won = gameWon;

		// Adding it to the vector (history) //
		history.push_back(data);
	}

	/* In this function we first generate a random number that will represent the computers number then we grab the users input for both
	betting amount and users number, after we run a for loop until the user runs out of tries, if the user wins we add the rewarded value
	to his balance depending on the game difficulty and break out the for loop, lastly once we are finished we save the results to the game history */
	void playComputer()
	{
		// Generating a random number using the random library which was recommended to me as up to date unlike the old method cstdlib //
		std::random_device device;
		std::mt19937 gen(device());
		std::uniform_int_distribution<> distr(0, difficultyLevel * 10);
		int computerNum = distr(gen);

		int betAmount = 0;
		int playerNum = 0;
		int tries = get_number_of_retries();

		while (true)
		{
			std::cout << "Enter Bet Amount: ";
			if (std::cin >> betAmount && currentCash >= betAmount && betAmount > 0)
			{
				break;
			}
			else
			{
				std::cout << "Invalid Number: Please try again" << '\n';
			}

			clearInputStream();
		}

		// Starting for loop that will end once the counter i (integer) reaches the max amount of tries the user has depending on difficulty //
		bool gameWon = false;
		for (int i = 0; i < tries; i++)
		{
			std::cout << "\nAttempt " << i + 1 << "/" << tries << " : Computer Number Ranges From (0 - " << std::to_string(difficultyLevel * 10) << ")\nEnter your number: ";
			if (!(std::cin >> playerNum))
			{
				std::cout << "Invalid Number: Please try again";
				clearInputStream();
				continue;
			}

			// Displaying the rewards to the user and that he was won the game //
			if (playerNum == computerNum)
			{
				std::cout << "\nCongratulations you have won the game" << '\n';
				std::cout << "Reward: $" << betAmount * difficultyLevel << '\n';
				std::cout << "Completed in " << i + 1 << "/" << tries << " tries" << '\n';
				
				currentCash += betAmount * difficultyLevel;
				gameWon = true;

				break;
			}
			else {
				std::cout << "\nWrong!\n";
			}
		}

		// If the player fails to guess the computers number we deduct the betting amount from his balance then continue to save the results //
		if (!gameWon)
		{
			std::cout << "\nYou have failed, no more tries left. the computers number was: " << computerNum << '\n';
			currentCash -= betAmount;
		}

		saveResults(difficultyLevel, betAmount, gameWon);
	}

	// Displaying the users game history data //
	void displayHistory()
	{
		// Looping through each struct inside the vector and printing out the value using std::cout //
		for (const gameResults& gameHistory : history)
		{
			std::cout << "Difficulty Level: " << gameHistory.difficultyLevel << '\n';
			std::cout << "Bet Amount: $" << gameHistory.betAmount << '\n';
			std::cout << "Game Won: " << std::boolalpha << gameHistory.won << "\n\n";
		}
	}

	/* This function is used to detect if the user has no cash left meaning returning false or if he does have cash to 
	play the game therefore returning true */
	bool validateCash()
	{
		if (currentCash <= 0)
			return false;
		else
			return true;
	}
};

/* Displaying options once the user has finished the game on whether he might want to continue playing with his current cash
or display the history of the results hes had in his past games or change the game difficulty then continue to play or lastly
exiting the program / casino */
int displayOptions()
{
	int minOption = 1;
	int maxOption = 4;
	int option;

	while (true)
	{
		std::cout << "\n1 = Continue Playing : 2 = Display History : 3 = Change Difficulty : 4 = Exit Casino" << '\n';
		std::cout << "Input: ";
		if (std::cin >> option && option >= minOption && option <= maxOption)
			break;
		else
			std::cout << "Invalid Input: Please try again" << '\n';
		clearInputStream();
	}

	return option;
}

// Main Entry - Handling casino game logic //
int main()
{
	// Greeting user and prompting that he needs to press enter to start the game //
	std::cout << "***** Casino Number Guessing Game ******\n\nPress Enter to begin" << '\n';
	std::cin.get();

	/* We will be using system("cls") to clear the console of all input which basiclly wipes the displayed
	code and making it easier to read and more professional but of course this is not required */
	system("cls");

	// Creating user1's class data for the minigame //
	UserData user1(9e9);
	user1.set_difficulty_level();
	clearInputStream();
	system("cls");

	// Starting a infinite while loop that will keep the game running until the user exits manually when prompted //
	int option = 1;
	while (true)
	{
		// If the user doesnt have enough cash to start the game which is anything above $0 we will early exit the program //
		if (!user1.validateCash() && option == 1)
		{
			std::cout << "Your out of cash, Better luck next time." << '\n';
			break;
		};

		// Option 1 means that we will play the game //
		if (option == 1)
		{
			std::cout << "**** Chosen Difficulty Level: " << user1.get_difficulty_level() << " ****\n\n";
			std::cout << "Cash Balance: $" << user1.get_cash_balance() << '\n';
			std::cout << "Number of tries: " << user1.get_number_of_retries() << "\n\n";

			user1.playComputer();
			option = displayOptions();
		}

		// Option 2 means we will display the results history of the users game //
		else if (option == 2)
		{
			system("cls");
			user1.displayHistory();
			option = displayOptions();
		}

		// Option 3 means we will simply change the difficulty then prompt the options again //
		else if (option == 3)
		{
			user1.set_difficulty_level();
			option = displayOptions();
		}

		// Option 4 or anything else means we will exit the program as the user most likely wants to quit //
		else
		{
			break;
		}

		system("cls");
	}

	// This is completly optional but i like to use to determine the end of the program //
	return 0;
}
