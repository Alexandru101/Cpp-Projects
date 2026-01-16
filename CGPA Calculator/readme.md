# CGPA Calculator Documentation

## What is a CGPA calculator?
Before starting this project i admit it was very confusing at first to understand what GPA even meant as i dont live near usa or other countries that use the GPA grading for school systems, however after reading some articles like this one --> https://www.timeshighereducation.com/student/advice/what-gpa and watching this example --> https://www.google.com/search?client=firefox-b-d&hs=cIq9&sca_esv=1af006d14f18ca7f&udm=7&sxsrf=ANbL-n4sxMSl454jkv00NZenz2XFGt6fYg:1768578416021&q=cgpa+calculator+c%2B%2B&sa=X&ved=2ahUKEwjg-vmBtJCSAxWwWUEAHQ5hLpgQ8ccDKAJ6BAgZEAQ&biw=1869&bih=955&aic=0#fpstate=ive&ip=1&vld=cid:d352d401,vid:uCvnDAHfdeA,st:0 you should get a general understanding of what is required.

Overall our goal is to get how many courses the user has taken then get the grade and the credit hours for those exams (Note. credit hours is part of the grading system and will affect the total gpa) then we optionally get the courses name to make it realistic (eg maths, english, computer science etc) then we apply this math formula below that will get the total gpa for the user.

```
//  grade * credit hours //
// --------------------- //
//     credit hours      //
```

## Step 1: Including Libraries
```
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <iomanip>
```

## Step 2: Input stream Management
- This function will reset the input stream by clearing the leftover whitespaces and reseting the errors if any from std::cin, lastly helps keeps us from having to keep rewriting the same code again.
```
void discardLine() {
	if (std::cin.fail())
		std::cin.clear();

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
```

## step 3: Defining the Data Structure
- Creating a struct that will hold the required variables for the user such as courseName, grade and creditHours. Essentially we can use this to create copies for each individual course the user has took.
```
struct coursesData {
	std::string courseName;
	int grade;
	int creditHours;
};
```

## Step 4: Capturing Course Count
- Grabbing the original integer named totalCourses using the & and then prompting the user with how much courses he has took, once the input is validated by the if statement we set the value to the totalCourses.
```
void getCourseAmount(int& totalCourses) {
	while (true) {
		std::cout << "Enter the amount of courses taken: ";
		if (std::cin >> totalCourses)
			break;
		else
			discardLine();
	}
}
```

## Step 5: Validating Course Statistics
- Taking the integer named type so that we can reuse this function for both credit hours and grade. Note we also wrap the code in a while loop so that we can validate the users input and once the input is valid and the type is validated (type 1 = Credit Hours, type 2 = Grade) we set the struct's value to the users input, lastly if anything fails we prompt the user with the failure message and break out the loop which leads to the function automatically destroying (freeing memory on the stack)
```
void setCourseStats(coursesData& course, int type) {
	while (true) {
		if (type == 1) {
			std::cout << "Credit Hours: ";
			if (std::cin >> course.creditHours)
				break;
			else
				discardLine();
			continue;
		}
		else if (type == 2) {
			std::cout << "Grade (0 - 4): ";
			if (std::cin >> course.grade)
				break;
			else
				discardLine();
			continue;
		}
		else {
			std::cout << "Failed to set course stats" << '\n';
			break;
		}
	}
}
```

## Step 6: Collecting Course Data & Memory Optimization
- Firstly we take 2 parameters that is the direct refrence to struct_list and totalCourses then immediatly entering a for loop after clearing the input stream using discardLine() which is optional but it wont hurt to use it, next we create a struct for each individual itereation inside the for loop and store the course name, credit hours and grade using the reusable function from above and manually getting the user input with std::getline.

- Once each iteration inside the for loop is at the last executable code we will push the struct to the vector which basically stores our struct for later so we can calculate the GPA at the end.
```
void getCourseData(std::vector <coursesData>& struct_list, int& totalCourses) {
	discardLine();

	for (int i = 0; i < totalCourses; i++) {
		coursesData user;	

		std::cout << "\nEnter course name: ";
		std::getline(std::cin, user.courseName);

		setCourseStats(user, 1);
		setCourseStats(user, 2);
		discardLine();

		struct_list.push_back(std::move(user));
	}
}
```

## Step 7: GPA Calculation Logic
- Looping through the vector and applying the formulat explained from above, once we are finished with the loop we return the calculated gpa.
```
double calculateGPA(const std::vector <coursesData>& struct_list) {
	double totalGradePoints = 0.0;
	int totalCreditHours = 0;

	for (const coursesData& course : struct_list) {
		totalGradePoints += course.grade * course.creditHours;
		totalCreditHours += course.creditHours;
	}

	if (totalCreditHours == 0)
		return 0.0;
	else
		return totalGradePoints / totalCreditHours;
}
```

## Step 8: Main Entry
We control the flow of the program and functions here then after we collect the end result (GPA) we prompt the user with the final result to the exact 0.00 decimal
```
int main()
{
	int totalCourses;

	std::cout << "CGPA Calculator. Press enter to start" << '\n';
	std::cin.get();
	getCourseAmount(totalCourses);

	std::vector <coursesData> stuct_list;
	stuct_list.reserve(totalCourses);
	getCourseData(stuct_list, totalCourses);

	double result = calculateGPA(stuct_list);
	std::cout << "Your total is: " << std::fixed << std::setprecision(2) << result << " GPA" << '\n';

	return 0;
}
```
