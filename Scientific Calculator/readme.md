# Scientific-Calculator Documentation

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

To follow BIDMAS order, we must prioritize multiplication and division over addition and subtraction, as well as giving sin, cos, tan, and log higher priority. Of course, we give parentheses () the highest priority by not including them in this specific function, as they are handled individually later in the calculate function.

We do this by using multiple if statements for each operator and returning a number that represents its importance. For example, if we catch addition or subtraction, we return 1 (lowest priority). If we catch multiplication or division, we return 2. Finally, sin, cos, tan, or log return 3, giving them the highest functional priority.

Note: Parentheses are always calculated first regardless of these values. We handle the parenthesis logic independently, meaning we bypass the priority check and simply prioritize them if they exist in the equation.

##

Finally, we solve the equation by taking two values and an operator. The function (named apply_operation_order) identifies the operator, performs the calculation independently, and returns the result to the calling function. This process is repeated until all values have been calculated and no operators remain in the stack (named operators).
```
class Calculator
{
private:
	std::vector<token> tokenList;

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
```
