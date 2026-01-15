// Including necessary libraries //
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>

/* Storing global game variables so we dont have to worry spelling error later on and can just refer to these //

// using const instead of constexpr because I learnt that you shouldnt use constexpr on strings because some compilers
// will fail to compile the program and as std::string is not a literal type it wont be known at compile time */

const std::string rock = "Rock";
const std::string paper = "Paper";
const std::string scissors = "Scissors";

// Generating a random response response from the computer //
std::string generate_computers_move()
{
	// Generating random number then comparing to return a result eg rock, scissors or paper //
	int randomNum = (rand() % 3);
	if (randomNum == 0)
	{
		return rock;
	}
	else if (randomNum == 1)
	{
		return paper;
	}
	else if (randomNum == 2)
	{
		return scissors;
	}

	// If for whatever reason the calculation failed we will exit the function with a warning string to check for later //
	return "Invalid Number";
}

// Comparing results then returning the winner of the game //
char generateWinner(const std::string& playerMove, const std::string& computerMove)
{
	// We are comparing every possible outcome then returning back a character letter to check for later if we Won, Loss or Drew //
	if (playerMove == rock && computerMove == paper)
	{
		return 'L';
	}
	else if (playerMove == rock && computerMove == rock)
	{
		return 'D';
	}
	else if (playerMove == rock && computerMove == scissors)
	{
		return 'W';
	}
	else if (playerMove == paper && computerMove == paper)
	{
		return 'D';
	}
	else if (playerMove == paper && computerMove == rock)
	{
		return 'W';
	}
	else if (playerMove == paper && computerMove == scissors)
	{
		return 'L';
	}
	else if (playerMove == scissors && computerMove == paper)
	{
		return 'W';
	}
	else if (playerMove == scissors && computerMove == rock)
	{
		return 'L';
	}
	else if (playerMove == scissors && computerMove == scissors)
	{
		return 'D';
	}

	/* If for whatever reason we didnt find a match we will return a character that will represant
	// the response being invalid so we can handle exiting the program later on */
	return '?';
}

// Creating the entry point of our program //
int main()
{
	// Initializing Timer For Random Number Generator //
	srand(time(nullptr));

	// Player Move Tracker //
	std::string playerMove;

	// Prompting User With Game Objects Selection //
	std::cout << "Welcome to rock / paper / scissors game.\tPress Enter to begin";
	std::cin.get();

	// Grabbing User Input //
	while (true) {
		std::cout << "Pick your game object: Rock | Paper | Scissors" << '\n';
		std::cin >> playerMove;
		if (playerMove == rock || playerMove == paper || playerMove == scissors)
			break;
		else
			std::cout << "Invalid Input: Please try again" << '\n';
	}

	// Generating Computers Move //
	std::string computerMove = generate_computers_move();
	if (computerMove == "Invalid Number")
	{
		std::cout << "\nComputer failed to generate move, please try again";
		return 0;
	}

	// Comparing End Results //
	char gameResults = generateWinner(playerMove, computerMove);
	if (gameResults == '?')
	{
		std::cout << "Game failed to calculate winner, please try again";
		return 0;
	}

	// Prompting user with the end results on who won the game then exiting program //
	if (gameResults == 'W')
	{
		std::cout << "\n\t\t\tCongratulation you won the game";
	}
	else if (gameResults == 'L')
	{
		std::cout << "\n\t\t\tComputer won the game, Better luck next time :/";
	}
	else if (gameResults == 'D')
	{
		std::cout << "\n\t\t\tGame Draw, Better luck next time";
	}

	// Optional return before program automatically exits as the end of the main entry //
	return 0;
}
