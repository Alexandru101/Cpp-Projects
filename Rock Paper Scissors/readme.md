# Rock-Paper-Scissors Game Documenation

![License](https://github.com/maxcomuk/Cpp-Projects/blob/main/LICENSE)

## Step 1: Include Necessary Libraries
- we Include all the necessary libraries we will use from visual studio 2022

```
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
```

## Step 2: Define Global Constants
- We create 3 constant variables that will represent rock, paper and scissors. The reason i do this, is so that we dont have to worry about spelling issues later on and it can end up being a bit ambigous having writing them again

- One thing i learnt is that we cant use constexpr because std::string is not a literal type therefore the compiler wont know the value at compile time wich ruins the purpose of constexpr and it can even lead to some compilers failing to build because of this, so definatly something we will avoid by simply changing constexpr to a const string
```
const std::string rock = "Rock";
const std::string paper = "Paper";
const std::string scissors = "Scissors";
```

## Step 3: Generate Computer’s Move
- Before starting this project i scanned through the project example wich was created here --> https://www.geeksforgeeks.org/cpp/rock-paper-scissor-game-in-cpp/ and after scanning through the project example i came to a conclusion that we will need to create a function that will basically generate a random response that we will later compare to get the winner.

- I learnt a nice optimization technique that we can do to skip loads of if statements using an array to peform a lookup later on therefore removing the need to do an if statement for every possible outcome

- Ultimatly all this function does is generate a random number from 0 to 2 (0, 1, 2) and then peforming a quick index lookup through the objects array wich we return the value of to the function that is calling this function eg the main entry function
```
std::string generate_computers_move()
{
	const std::string objects[] = { rock, paper, scissors };

	int randomNum = (rand() % 3);
	return objects[randomNum];
}
```

## Step 4: Determine the Winner
- This is the function that will compare both results eg (players move and computers move) then after returning a character that will represent a loss, win or a draw. Note loads of if statement are peformed here to return all the possible outcomes although it can be a bit rebundant it gets the job done when you dont have loads of outcomes as a simple rock-paper-scissors game

- It should be pretty self-explanatory but (L = Loss, W = Won, D = Draw)
```
char generateWinner(const std::string& playerMove, const std::string& computerMove)
{
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

	return '?';
}
```

## Step 5: Main Program
- This is the main entry point of the program and where all the logic is peformed

- Overall we are basically grabbing the users input in a while loop and check if the input is valid if its invalid it will keep repeating until input / move is valid, once the user has inserted a valid type such as Rock we break out of the loop and generate a random response from the computer, once we have both a response from the user and the computer we will use the generateWinner function that will take 2 paremeters eg the player move and the computers move

- After the function finishes comparing both responses it will return a character that represents the game state from above, we store that character in a char variable named gameResults wich really shouldnt have an 's' at the end as its not a plurar name but ye we will call it gameResults for now

- Lastly we will peform 3 if statements that will prompt the user if he has Won, Loss or drawed with the computer. Note i put (return 0) at the end but that is completly optional although i dont know much about other compilers and if they require it
```
int main()
{
	srand(time(nullptr));

	std::string playerMove;

	std::cout << "Welcome to rock / paper / scissors game.\tPress Enter to begin";
	std::cin.get();

	while (true) {
		std::cout << "Pick your game object: Rock | Paper | Scissors" << '\n';
		std::cin >> playerMove;
		if (playerMove == rock || playerMove == paper || playerMove == scissors)
			break;
		else
			std::cout << "Invalid Input: Please try again" << '\n';
	}

	std::string computerMove = generate_computers_move();

	char gameResults = generateWinner(playerMove, computerMove);
	if (gameResults == '?')
	{
		std::cout << "Game failed to calculate winner, please try again";
		return 0;
	}

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

	return 0;
}
```

# Installation Proccess
- Copy and paste method -
Copy the whole main.cpp file and then paste it into your ide then compile it

- File download method -
Download the file and then simply open it with your prefferd ide to compile
