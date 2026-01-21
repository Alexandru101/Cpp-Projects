# Scientific-Calculator Documentation

![License](https://github.com/maxcomuk/Cpp-Projects/blob/main/LICENSE)

### What is a scientific calculator and what is the difference from a normal calculator?
A scientific calculator allows multiplication, division, addition and subtracting just like a normal calculator as well as following BIDMAS order. however a scientific calculator also gives you extra functionalities such as sin, cos, tan and log, which is not supported by a normal calculator.

The goal of this project is to support normal calculator functions while also including scientific calculator functions such as sin, cos, tan and log.

## Step 1: Including Libraries
```
#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <stack>
#include <cmath>
```

## Step 2: Defining Enums & Struct Data
We must first create a struct (named Token) to give each character from the user's equation a type and a value. Essentially, we are defining if a character is an operator (like +) or a number. We need to differentiate between the types of characters we are dealing with—whether it is a parenthesis, a number, etc. This is why we also create an enum named TokenType containing all possible character types.

In theory, we use the Token struct as a template to store each character's value and type in a vector inside our calculator class below. The main goal is to define the character type so we can separate it later into lists of numbers and operators. Note: when I say operators I mean +, -, *, /, and ().
```
enum tokenType
{
	NUMBER,
	OPERATOR,
	FUNCTION,
	PARENTHESIS
};

struct token
{
	tokenType type;
	std::string value;
};
```

## Step 3: Building the Calculator Class and Private Helper Methods
Next, we create a class (named Calculator) that handles organizing the user's string/equation into a token list. This list is a std::vector type that uses the Token struct as a template to store each character's type and value.

##

To populate the std::vector<token> tokenList with every character of the equation, we create a function named tokenise. This function takes the user's input string and firstly clears any previous data from the tokenList to ensure a clean slate. Next, it iterates through the userInput using a for loop, performing multiple conditional checks to separate each character (or group of characters) into its specific type and assign its corresponding value.

##

To follow BIDMAS order, we must prioritize multiplication and division over addition and subtraction, as well as giving sin, cos, tan, and log higher priority. Of course, we give parentheses () the highest priority by not including them in this specific function, as they are handled individually later in the calculate function.

We do this by using multiple if statements for each operator and returning a number that represents its importance. For example, if we catch addition or subtraction, we return 1 (lowest priority). If we catch multiplication or division, we return 2. Finally, sin, cos, tan, or log return 3, giving them the highest functional priority.

Note: Parentheses are always calculated first regardless of these values. We handle the parenthesis logic independently, meaning we bypass the priority check and simply prioritize them if they exist in the equation.

##

Finally, we solve the equation by taking two values and an operator. The function (named apply_operation_order) identifies the operator, performs the calculation independently using if statements to define what operator it is, and returns the result to the calling function. This process is repeated until all values have been calculated and no operators remain in the stack (named operators).

##

By utilizing the cmath library, we can easily implement sin, cos, tan, and log calculations. We achieve this by taking the operator and its associated value, then using conditional if statements to determine which std:: method to call before returning the final result to the calling function.

##

Lastly to process the equation, we create a function named processMath that manages both the values and operators stacks. This function performs a conditional check to determine which operator is at the top of the stack. Depending on the operator type, it calls the appropriate function to calculate either a single value (for scientific functions) or two values (for basic arithmetic). The result is then stored back into the values stack. This process continues until only one value remains inside the values stack, which represents the final result of the equation.
```
class Calculator
{
private:
	std::vector<token> tokenList;

	void tokenise(const std::string& userInput)
	{
		tokenList.clear();

		for (int i = 0; i < userInput.length(); i++)
		{
			char c = userInput[i];
			if (isspace(c)) { continue; }

			if (isdigit(c) || c == '.')
			{
				std::string num;
				while (i < userInput.length() && (isdigit(userInput[i]) || userInput[i] == '.'))
				{
					num += userInput[i];
					i++;
				}
				i--;
				tokenList.push_back({ NUMBER, num });
			}
			else if (isalpha(c))
			{
				std::string func;
				while (i < userInput.length() && isalpha(userInput[i]))
				{
					func += userInput[i];
					i++;
				}
				i--;
				tokenList.push_back({ FUNCTION, func });
			}
			else if (c == '+' || c == '-' || c == '*' || c == '/')
			{
				tokenList.push_back({ OPERATOR, std::string(1, c) });
			}
			else if (c == '(' || c == ')')
			{
				tokenList.push_back({ PARENTHESIS, std::string(1, c) });
			}
		}
	}

	const int precedence(const std::string& op) const
	{
		if (op == "+" || op == "-") { return 1; }
		if (op == "*" || op == "/") { return 2; }
		if (op == "sin" || op == "cos" || op == "tan" || op == "log") { return 3; }

		return 0;
	}

	const double apply_operation_order(double a, double b, const std::string& op) const
	{
		if (op == "+") { return a + b; }
		if (op == "-") { return a - b; }
		if (op == "*") { return a * b; }
		if (op == "/") { return a / b; }

		return 0;
	}

	const double applyFunc(double value, const std::string& func)
	{
		if (func == "sin") { return std::sin(value); }
		if (func == "cos") { return std::cos(value); }
		if (func == "tan") { return std::tan(value); }
		if (func == "log") { return std::log(value); }

		return value;
	}

	void processMath(std::stack<double>& values, std::stack<std::string>& operators)
	{
		std::string op = operators.top(); operators.pop();

		if (op == "sin" || op == "cos" || op == "tan" || op == "log")
		{
			double val = values.top(); values.pop();
			values.push(applyFunc(val, op));
		}
		else {
			double val2 = values.top(); values.pop();
			double val1 = values.top(); values.pop();
			values.push(apply_operation_order(val1, val2, op));
		}
	}
```

## Step 4: Implementing the Final Calculation Logic
Now that we have all the necessary methods and functions we need to organise and calculate, we create a function named Calculate that will handle the whole logic from taking the users input and sorting it then calculating it accordingly. We do this firstly by filling the tokenList with all the character when calling tokenise and then creating two stack variables that will contain the values and the operators. Note its important its a stack and not an array or a vector because we need it to go in stack order to peform each calculation accordingly. Next we run a for loop through the entire tokenList and call processMath to calculate the two values or one using the operator that is from the operators stack, one more thing I forgot to mention is that we also populate the operators stack with the operators inside tokenList and also populate the values stack with the results of the calculations 

Now that we have all the necessary methods and functions needed to organize and calculate, we create a function named Calculate. This function handles the entire logic, from taking the user's input and sorting it to calculating the final result.

We do this by first filling the tokenList with all characters by calling tokenise. Then, we create two stack variables to hold the values and the operators. Note that it is important to use a stack rather than an array or a vector because we need the "Last-In, First-Out" order to perform each calculation in the correct sequence.

Next, we run a for loop through the entire tokenList. We populate the operators stack with operators from the list and the values stack with numbers. As we go, we call processMath to calculate the values using the operators from the stack, eventually storing the final results back into the values stack until we have a finaly result eg only one value in values stack.

Lastly we use std::cout to print the answear to the users equation but of course you can change this function from a void to an int to return the value and modify it inside the main function if you want.
```
public:
	void Calculate(std::string& userInput)
	{
		tokenise(userInput);

		std::stack<double> values;
		std::stack<std::string> operators;

		for (int i = 0; i < tokenList.size(); i++)
		{
			if (tokenList[i].type == NUMBER)
			{
				values.push(std::stod(tokenList[i].value));
			}
			else if (tokenList[i].type == PARENTHESIS && tokenList[i].value == "(")
			{
				operators.push("(");
			}
			else if (tokenList[i].type == PARENTHESIS && tokenList[i].value == ")")
			{
				while (!operators.empty() && operators.top() != "(")
				{
					processMath(values, operators);
				}
				if (!operators.empty()) { operators.pop(); }
			}
			else if (tokenList[i].type == OPERATOR)
			{
				while (!operators.empty() && precedence(operators.top()) >= precedence(tokenList[i].value))
				{
					processMath(values, operators);
				}

				operators.push(tokenList[i].value);
			}
			else if (tokenList[i].type == FUNCTION)
			{
				operators.push(tokenList[i].value);
			}
		}

		while (!operators.empty())
		{
			processMath(values, operators);
		}

		if (!values.empty())
		{
			std::cout << "Result: " << values.top() << "\n";
			std::cout << "[Exit] = Exit Program" << "\n\n";
		}
		else
		{
			std::cout << "Values not found please try again" << '\n';
		}
	}
};
```

## Step 5: Validating UserInput
While we know its a calculator and wont be typing in random strings its important to still try to protect the program from crashing incase we mistype, however this function that i have created is not perfect and only works to an extent as if the user types sin, cos, tan or log then they can type whatever they want after like a random character like = or number of characters like 0askfa which will cause the program to crash, if you wanted to refine this you could 

Although this is a calculator, it is vital to protect the program from crashing due to typos or invalid strings. I have created a function named validateString to act as a first line of defense. It uses a whitelist system to ensure that only mathematical characters, numbers, and supported function names are processed.

However, this validation is weak, while it catches invalid symbols (like @ or !), it doesn't yet verify the syntax. For example, a user could type a valid function followed by random characters that are also on the whitelist, which could still lead to a crash during calculation. To secure the program effeciantly, further logic would be needed to ensure parentheses are balanced and operators aren't placed side-by-side.
```
bool validateString(const std::string& userInput)
{
	std::string whitelistedChars = "1234567890.sincostanlog()*/+- ";

	for (const char& c : userInput)
	{
		if (whitelistedChars.find(c) == std::string::npos)
			return false;
	}

	return true;
}
```

Step 6: Main Entry Point

The main function serves as the entry point of the program, managing the main loop and handling user interaction.

First, we instantiate our Calculator class and create a string variable to store the user's input. After prompting the user to press enter to begin, we initiate a while loop that continues forever until the user types "Exit." During each iteration, the program captures the user's equation. If the input is not the exit command and passes the validation check, we call the Calculate method from our class to process the equation and display the result.
```
int main()
{
	std::string calculation;
	Calculator calculator;

	std::cout << "**** Scientific Calculator ****" << '\n';
	std::cout << "-->Press Enter to begin . . ." << "\n";
	std::cin.get();

	while (true)
	{
		std::cout << "\nInput: ";
		std::getline(std::cin, calculation);

		for (char& c : calculation) { c = tolower(c); }

		if (calculation.find("exit") != std::string::npos)
			break;

		if (validateString(calculation))
		{
			calculator.Calculate(calculation);
		}
		else
		{
			std::cout << "Invalid Equation!" << '\n';
		}
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
