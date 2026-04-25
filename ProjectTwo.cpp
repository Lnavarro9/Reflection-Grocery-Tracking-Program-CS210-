#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Course structure
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Trim whitespace, quotes, and hidden BOM characters
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n\"'");
    if (first == string::npos) {
        return "";
    }

    size_t last = str.find_last_not_of(" \t\r\n\"'");
    string result = str.substr(first, last - first + 1);

    // Remove UTF-8 BOM if present
    if (result.size() >= 3 &&
        static_cast<unsigned char>(result[0]) == 0xEF &&
        static_cast<unsigned char>(result[1]) == 0xBB &&
        static_cast<unsigned char>(result[2]) == 0xBF) {
        result = result.substr(3);
    }

    return result;
}

// Convert a string to uppercase
string toUpperCase(string str) {
    transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c) { return static_cast<char>(toupper(c)); });
    return str;
}

// Split one CSV line into tokens
vector<string> splitLine(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        token = trim(token);

        // Ignore empty tokens caused by trailing commas
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

// Check whether a course exists
bool courseExists(const vector<Course>& courses, const string& courseNumber) {
    string target = toUpperCase(trim(courseNumber));

    for (const Course& course : courses) {
        if (toUpperCase(course.courseNumber) == target) {
            return true;
        }
    }

    return false;
}

// Get the title for a course number
string getCourseTitle(const vector<Course>& courses, const string& courseNumber) {
    string target = toUpperCase(trim(courseNumber));

    for (const Course& course : courses) {
        if (toUpperCase(course.courseNumber) == target) {
            return course.courseTitle;
        }
    }

    return "Unknown Course";
}

// Load courses from file
bool loadCoursesFromFile(const string& fileName, vector<Course>& courses) {
    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file \"" << fileName << "\"." << endl;
        return false;
    }

    vector<vector<string>> rawLines;
    string line;

    // First pass: read file and validate basic structure
    while (getline(inputFile, line)) {
        line = trim(line);

        if (line.empty()) {
            continue;
        }

        vector<string> tokens = splitLine(line);

        if (tokens.size() < 2) {
            cout << "Error: Invalid file format." << endl;
            inputFile.close();
            return false;
        }

        rawLines.push_back(tokens);
    }

    inputFile.close();

    if (rawLines.empty()) {
        cout << "Error: File is empty." << endl;
        return false;
    }

    courses.clear();

    // Second pass: create all course objects
    for (const vector<string>& tokens : rawLines) {
        Course course;
        course.courseNumber = trim(tokens[0]);
        course.courseTitle = trim(tokens[1]);
        courses.push_back(course);
    }

    // Third pass: validate prerequisites and assign them
    for (size_t i = 0; i < rawLines.size(); ++i) {
        for (size_t j = 2; j < rawLines[i].size(); ++j) {
            string prereq = trim(rawLines[i][j]);

            if (prereq.empty()) {
                continue;
            }

            if (!courseExists(courses, prereq)) {
                cout << "Error: Invalid prerequisite for course "
                    << rawLines[i][0] << endl;
                courses.clear();
                return false;
            }

            courses[i].prerequisites.push_back(prereq);
        }
    }

    cout << "Course data loaded successfully." << endl;
    return true;
}

// Print all courses in alphanumeric order
void printCourseList(const vector<Course>& courses) {
    if (courses.empty()) {
        cout << "No course data loaded." << endl;
        return;
    }

    vector<Course> sortedCourses = courses;

    sort(sortedCourses.begin(), sortedCourses.end(),
        [](const Course& a, const Course& b) {
            return toUpperCase(a.courseNumber) < toUpperCase(b.courseNumber);
        });

    cout << "\nHere is a sample schedule:" << endl;
    for (const Course& course : sortedCourses) {
        cout << course.courseNumber << ", " << course.courseTitle << endl;
    }
}

// Print one course and its prerequisites
void printCourseInformation(const vector<Course>& courses, const string& courseNumber) {
    if (courses.empty()) {
        cout << "No course data loaded." << endl;
        return;
    }

    string target = toUpperCase(trim(courseNumber));

    for (const Course& course : courses) {
        if (toUpperCase(course.courseNumber) == target) {
            cout << "\n" << course.courseNumber << ", " << course.courseTitle << endl;

            if (course.prerequisites.empty()) {
                cout << "Prerequisites: None" << endl;
            }
            else {
                cout << "Prerequisites: ";

                for (size_t i = 0; i < course.prerequisites.size(); ++i) {
                    cout << course.prerequisites[i]
                        << ", " << getCourseTitle(courses, course.prerequisites[i]);

                    if (i < course.prerequisites.size() - 1) {
                        cout << "; ";
                    }
                }

                cout << endl;
            }

            return;
        }
    }

    cout << "Error: Course " << courseNumber << " not found." << endl;
}

// Print menu
void printMenu() {
    cout << "\nMenu:" << endl;
    cout << "  1. Load Data Structure" << endl;
    cout << "  2. Print Course List" << endl;
    cout << "  3. Print Course" << endl;
    cout << "  9. Exit" << endl;
    cout << "\nWhat would you like to do? ";
}

int main() {
    vector<Course> courses;
    string fileName;
    int choice = 0;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        printMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: Please enter a valid menu option." << endl;
            continue;
        }

        cin.ignore(1000, '\n');

        switch (choice) {
        case 1:
            cout << "Enter file name: ";
            getline(cin, fileName);

            if (trim(fileName).empty()) {
                cout << "Error: File name cannot be blank." << endl;
            }
            else {
                loadCoursesFromFile(fileName, courses);
            }
            break;

        case 2:
            printCourseList(courses);
            break;

        case 3: {
            string courseNumber;
            cout << "What course do you want to know about? ";
            getline(cin, courseNumber);

            if (trim(courseNumber).empty()) {
                cout << "Error: Course number cannot be blank." << endl;
            }
            else {
                printCourseInformation(courses, courseNumber);
            }
            break;
        }

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}