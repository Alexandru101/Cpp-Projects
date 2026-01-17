# Casino-Guessing-Game Documentation

## Step 1: Including Libraries
```
#include <iostream>
#include <limits>
#include <random>
#include <vector>
#include <string>
```

## Step 2
This function will be used to clear the input stream of any errors that might occur from the user typing, for example if we request a difficulty level we want the user to input a number but if he types a bunch of characters then that will cause errors to the input stream thanks to this function we can clear those errors and also remove any leftover whitespaces to use the next available line.
```
void clearInputStream()
{
	if (std::cin.fail())
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
```

## Step 3
This struct will be used as a template for when we save data to the game history. (we use a vector to store these values into a list later on)
```
struct gameResults
{
	int difficultyLevel;
	int betAmount;
	bool won;
};
```

## Step 4
We first create a class that will contain all the methods required to play the minigame this will also be used to hold properties / data for the user.

The reason we contain our data inside the private section is so that anyone reading this code or if we make updates later on we wont tamper with any of the values as they will be important for the users minigame and if changed incorrectly it can mess with the games state.

Next we create 2 constructors, the first being if the user has provided no number for when creating the class that will be used for the starting cash we will manually initialize the currentCash variable to the default starting cash of $100, the second constructor is used if we provide a starting cash amount (integer) in the parameter when creating the class this will ultimately set the starting cash to the modified value which allows us to config our game how we want it.

Lastly we create multiple methods that will be used when specified by the main entry function at the end, First function we create is called set_difficulty_level and this is purely designed to prompt the user with what difficulty he wishes to have then once we validate the input as a integer and a number from 1 to 3 we will set the difficulty level to this and break out of the while loop, Secondly we create 3 mini functions that will be required if we want to access out currents game difficulty level and the current cash from a outside scope eg the main entry later on, Note we also calculate the number of retries through the get_number_of_retries function and then return the amount of retires the user has.

After this we make a function that will save the currents game result and add it to the vector (list) that will be used to displayed later if the user wishes to see his current game history results, to do this we first instantiate a new struct and set the template values to the games results which will be given through the required parameters then simply use push_back to add it to the vector

Once we have done this now we need to create a method that will handle the game logic which will include grabbing the players betting amount and number then after we have generated a random number we compare the numbers and prompt the user if he has won the game or lost and of course add the rewards, lastly we save the results of this game exit the function.

The last 2 functions are displayHistory which simply loops through the vector and displays the stats for that game and validateCash which just returns false if the user has $0 or returns true if he has more than $0, this is useful so we can determine if the user can still play the minigame
```
class UserData
{
private:
	int difficultyLevel;
	int currentCash;

	const int maxLevel{ 3 };
	const int minLevel{ 1 };

	std::vector <gameResults> history;
public:
	UserData() : currentCash{ 100 } {};
	UserData(const int startingCash) : currentCash{ startingCash } {};

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

	int get_difficulty_level() const { return difficultyLevel; };

	int get_cash_balance() const { return currentCash; };

	int get_number_of_retries() const
	{
		if (difficultyLevel == 1) { return 3; };
		if (difficultyLevel == 2) { return 2; };

		return 1;
	}

	void saveResults(const int& difficulty, const int& amount, const bool& gameWon)
	{
		gameResults data;

		data.difficultyLevel = difficulty;
		data.betAmount = amount;
		data.won = gameWon;

		history.push_back(data);
	}

	void playComputer()
	{
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

		if (!gameWon)
		{
			std::cout << "\nYou have failed, no more tries left. the computers number was: " << computerNum << '\n';
			currentCash -= betAmount;
		}

		saveResults(difficultyLevel, betAmount, gameWon);
	}

	void displayHistory()
	{
		for (const gameResults& gameHistory : history)
		{
			std::cout << "Difficulty Level: " << gameHistory.difficultyLevel << '\n';
			std::cout << "Bet Amount: $" << gameHistory.betAmount << '\n';
			std::cout << "Game Won: " << std::boolalpha << gameHistory.won << "\n\n";
		}
	}

	bool validateCash()
	{
		if (currentCash <= 0)
			return false;
		else
			return true;
	}
};
```

## Step 5
Once the user has finished his game this function will be used to display what options he can do next and return the choice, for example the user wishes to continue playing he would enter 1 and so on.
```
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
```

## Step 6
This is where we handle all the games logic and use all the functions and class methods

Main purpose of this function is to start the game and then run a conditional while loop that we will exit only if the user doesnt have enough cash to keep playing or if he exits the program when giving the option to
```
int main()
{
	std::cout << "***** Casino Number Guessing Game ******\n\nPress Enter to begin" << '\n';
	std::cin.get();

	system("cls");

	UserData user1;
	user1.set_difficulty_level();
	clearInputStream();
	system("cls");

	int option = 1;
	while (true)
	{
		if (!user1.validateCash() && option == 1)
		{
			std::cout << "Your out of cash, Better luck next time." << '\n';
			break;
		};

		if (option == 1)
		{
			std::cout << "**** Chosen Difficulty Level: " << user1.get_difficulty_level() << " ****\n\n";
			std::cout << "Cash Balance: $" << user1.get_cash_balance() << '\n';
			std::cout << "Number of tries: " << user1.get_number_of_retries() << "\n\n";

			user1.playComputer();
			option = displayOptions();
		}

		else if (option == 2)
		{
			system("cls");
			user1.displayHistory();
			option = displayOptions();
		}

		else if (option == 3)
		{
			user1.set_difficulty_level();
			option = displayOptions();
		}

		else
		{
			break;
		}

		system("cls");
	}

	return 0;
}
```

# Installation Process
- Copy & Paste Method -
Simply copy the whole of Main.cpp file and then paste it in your prefferd ide to compile

- File Download Method -
Download Main.cpp then open it with your prefferd ide to compile it

# Ide I used [Visual studio 2022](https://www.techspot.com/downloads/7493-visual-studio-2022.html)
