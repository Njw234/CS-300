// Nick Williams
// CS 300
// Project Two
// 10/15/2025


#include <algorithm>
#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Course {
	string number;
	string title;
	vector<string> prerequisites;

	Course() = default;
	Course(string num, string ttl) : number(std::move(num)), title(std::move(ttl)) {}
};

// Course catalog keyed by course number
using Catalog = map<string, Course>; // Course number -> Course



//Trim whitespace from both ends of a string
static inline void trim(string& s) {
	auto notSpace = [](unsigned char ch) {return !std::isspace(ch); };
	//left trim
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
	//right trim
	s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());

}

//Covert string to uppercase
static inline string upperCopy(string s) {
	for (auto& ch : s) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
	return s;
}

// Split a line on commas, trimming whitespace from each field

static vector<string> splitCSV(const string& line) {
	vector<string> out;
	string token;
	std::stringstream ss(line);
	while (std::getline(ss, token, ',')) {
		trim(token);
		out.push_back(token);
	}
	return out;
}

// Read file and populate catalog
bool loadDataStructure(const string& filename, Catalog& catalog, vector<string>& errors) {
	catalog.clear();
	errors.clear();

	ifstream fin(filename);
	if (!fin.is_open()) {
		errors.push_back("Could not open file: " + filename);
		return false;
	}

	string line;
	size_t lineNo = 0;
	set<string> seen; // To track duplicate course numbers

	while (std::getline(fin, line)) {
		++lineNo;
		if (line.empty()) continue; // Skip empty lines

		//remove potential carriage 
		if (!line.empty() && line.back() == '\r') line.pop_back();

		auto fields = splitCSV(line);
		if (fields.empty()) continue; // Skip empty lines

		//Require at least course number and title
		if (fields.size() < 2) {
			errors.emplace_back("Line " + to_string(lineNo) + ": Not enough fields");
			continue;
		}

		string number = upperCopy(fields[0]);
		string title = fields[1];	

		if (number.empty()) {
			errors.emplace_back("Line " + to_string(lineNo) + ": Missing course number");
			continue;
		}
		if (title.empty()) {
			errors.emplace_back("Line " + to_string(lineNo) + ": Missing course title");
			continue;
		}
		if (seen.count(number)) {
			errors.emplace_back("Line " + to_string(lineNo) + ": Duplicate course number: " + number);
			continue;
		}

		Course c{ number, title };
		// Process prerequisites if any
		for (size_t i = 2; i < fields.size(); ++i) {
			string prereq = upperCopy(fields[i]);
			if (!prereq.empty()) {
				c.prerequisites.push_back(prereq);
			}
		}

		catalog[number] = std::move(c);
		seen.insert(number);

	}

	if (catalog.empty()) {
		errors.push_back("No valid course data found in file: " + filename);
		return false;
	}
	return true;

}


// Print all courses in alphanumeric order by course number.
void printCourseList(const Catalog& catalog) {
	cout << "\nHere is a sample schedule:\n\n";
	for (const auto& kv : catalog) {              // kv.first = number, kv.second = Course
		cout << kv.first << ", " << kv.second.title << '\n';
	}
	cout << '\n';
}


//Print details for a single course
void printCourseInfo(const Catalog& catalog, string courseNumber) {
	courseNumber = upperCopy(courseNumber);
	auto it = catalog.find(courseNumber);
	if (it == catalog.end()) {
		cout << "Course " << courseNumber << " not found in the catalog.\n";
		return;
	}
	const Course& c = it->second;
	cout << c.number << "," << c.title << '\n';
	cout << "Prerequisites: ";
	if (c.prerequisites.empty()) {
		cout << "None\n";
	}
	else {
		for (size_t i = 0; i < c.prerequisites.size(); ++i) {
			if (i > 0) cout << ", ";
			cout << c.prerequisites[i];
		}
		cout << '\n';
	}
}

// Main program loop

int readIntChoice() {
	string input;
	std::getline(cin, input);
	//trim whitespace
	trim(input);
	if (input.empty()) return -1;
	//ensure all characters are digits
	for (char ch : input) if (!std::isdigit(static_cast<unsigned char>(ch))) return -1;
	try {
		return std::stoi(input);
	}
	catch (...) {
		return -1;
	}
}

int main() {
	Catalog catalog;
	vector<string> errors;
	bool dataLoaded = false;

	cout << "Welcome to the Course Catalog Manager!\n";
	int choice = 0;
	while (choice != 9) {
		cout << "\nMenu:\n"
			<< "1. Load Data Structure\n"
			<< "2. Print Course List\n"
			<< "3. Print Course\n"
			<< "9. Exit\n"
			<< "Enter choice: ";
		choice = readIntChoice();
		switch (choice) {
		case 1: {
			cout << "Enter file name to load (e.g., ABCU_Advising_Program_Input.csv): ";
			string filename;
			getline(cin, filename);
			trim(filename);

			if (filename.empty()) {
				cout << "No file name entered.\n\n";
				break;
			}

			bool ok = loadDataStructure(filename, catalog, errors);
			if (ok) {
				dataLoaded = true;
				cout << "Data loaded successfully (" << catalog.size() << " courses).\n\n";
			}
			else {
				dataLoaded = false;
				for (const auto& e : errors) cout << e << '\n';
				cout << '\n';
			}
			break;
		}

		case 2: {
			if (!dataLoaded) {
				cout << "No data loaded. Please load a data structure first.\n\n";
				break;
			}
			printCourseList(catalog);
			break;
		}

		case 3: { 
			if (!dataLoaded) {
				cout << "No data loaded. Please load a data structure first.\n\n";
				break;
			}
			cout << "Enter course number: ";
			string courseNumber; 
			getline(cin, courseNumber);
			trim(courseNumber);
			if (courseNumber.empty()) {
				cout << "Course number cannot be empty.\n\n";
				break;
			}
			printCourseInfo(catalog, courseNumber);
			break;
		}

		case 9:
			cout << "Exiting program. Goodbye!\n";
			break;

		default:
			cout << "Invalid choice. Please enter a valid option from the menu.\n\n";
			break;
		}
		return 0;
	}
}