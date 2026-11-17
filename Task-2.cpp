#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <stdexcept>
using namespace std;
 
class Student {
protected:
    int rollNo;
    string name;
 
public:
    Student(int roll, const string& studentName)
        : rollNo(roll), name(studentName) {}
 
    virtual void display() const = 0;   

    virtual ~Student() = default;
};
 
class Result : public Student {
private:
    int marks1, marks2, marks3;
 
public:
    Result(int roll, const string& studentName, int m1, int m2, int m3)
        : Student(roll, studentName),
          marks1(m1), marks2(m2), marks3(m3) {
        validateMark(marks1);
        validateMark(marks2);
        validateMark(marks3);
    }
 
    static void validateMark(int mark) {
        if (mark < 0 || mark > 100)
            throw out_of_range("Marks must be between 0 and 100.");
    }
 
    int total() const {
        return marks1 + marks2 + marks3;
    }
 
    double percentage() const {
        return total() / 3.0;
    }
 
    string resultStatus() const {
        return (marks1 >= 35 && marks2 >= 35 && marks3 >= 35)
                   ? "PASS" : "FAIL";
    }
 
    void writeRecord(ofstream& out) const {
        out << rollNo << '|' << name << '|'
            << marks1 << '|' << marks2 << '|' << marks3 << '\n';
    }
 
    void display() const override {
        cout << left << setw(8) << rollNo
             << setw(20) << name
             << right << setw(8) << marks1
             << setw(8) << marks2
             << setw(8) << marks3
             << setw(9) << total()
             << setw(12) << fixed << setprecision(2) << percentage()
             << setw(10) << resultStatus() << '\n';
    }
};
 
int readInteger(const string& prompt) {
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
 
        try {
            size_t used = 0;
            int value = stoi(input, &used);
            if (used != input.size())
                throw invalid_argument("Enter a whole number.");
            return value;
        } catch (const exception&) {
            cout << "Invalid input. Please enter a valid integer.\n";
        }
    }
}
 
int readMark(const string& subject) {
    while (true) {
        int mark = readInteger("Enter marks for " + subject + " (0-100): ");
        try {
            Result::validateMark(mark);
            return mark;
        } catch (const out_of_range& e) {
            cout << "Invalid marks: " << e.what() << " Try again.\n";
        }
    }
}
 
void printHeading() {
    cout << left << setw(8) << "Roll No"
         << setw(20) << "Name"
         << right << setw(8) << "Sub1"
         << setw(8) << "Sub2"
         << setw(8) << "Sub3"
         << setw(9) << "Total"
         << setw(12) << "Percent"
         << setw(10) << "Result" << '\n';
    cout << string(89, '-') << '\n';
}
 
void displayAll(const vector<Result>& students) {
    if (students.empty()) {
        cout << "No student records available.\n";
        return;
    }
 
    printHeading();
    for (const Result& student : students) {
        const Student* basePtr = &student;
        basePtr->display();
    }
}
 
void addStudent(vector<Result>& students) {
    int roll = readInteger("Enter roll number: ");
 
    cout << "Enter student name: ";
    string name;
    getline(cin, name);
 
    int m1 = readMark("Subject 1");
    int m2 = readMark("Subject 2");
    int m3 = readMark("Subject 3");
 
    try {
        students.emplace_back(roll, name, m1, m2, m3);
        cout << "Student record added successfully.\n";
    } catch (const exception& e) {
        cout << "Record could not be added: " << e.what() << '\n';
    }
}
 
void saveToFile(const vector<Result>& students) {
    ofstream outFile("students.txt");
    if (!outFile) {
        cout << "Unable to open file for writing.\n";
        return;
    }
 
    for (const Result& s : students) {

        s.writeRecord(outFile);
    }
    cout << "Records saved to students.txt successfully.\n";
}
 
void loadFromFile(vector<Result>& students) {
    ifstream inFile("students.txt");
    if (!inFile) {
        cout << "No saved file found (students.txt).\n";
        return;
    }
 
    vector<Result> loaded;
    string line;
    int skipped = 0;
 
    while (getline(inFile, line)) {
        try {
            stringstream ss(line);
            string rollText, name, m1Text, m2Text, m3Text;
            if (!getline(ss, rollText, '|') ||
                !getline(ss, name, '|') ||
                !getline(ss, m1Text, '|') ||
                !getline(ss, m2Text, '|') ||
                !getline(ss, m3Text, '|')) {
                throw invalid_argument("Incomplete record.");
            }
 
            int roll = stoi(rollText);
            int m1 = stoi(m1Text);
            int m2 = stoi(m2Text);
            int m3 = stoi(m3Text);
            loaded.emplace_back(roll, name, m1, m2, m3);
        } catch (const exception&) {
            ++skipped;
        }
    }
 
    students = loaded;
    cout << students.size() << " record(s) loaded from students.txt.";
    if (skipped > 0)
        cout << " " << skipped << " invalid record(s) skipped.";
    cout << '\n';
}
 
void filterStudents(const vector<Result>& students, int threshold) {
    vector<Result> selected;
 
    copy_if(students.begin(), students.end(), back_inserter(selected),
            [threshold](const Result& s) {
                return s.total() > threshold;
            });
 
    cout << "\nStudents with total marks above " << threshold << ":\n";
    if (selected.empty()) {
        cout << "No students found above this mark.\n";
        return;
    }
    printHeading();
    for (const Result& s : selected)
        s.display();
}
 
int main() {
    vector<Result> students;
    int choice;
 
    do {
        cout << "\n===== STUDENT RESULT MANAGEMENT SYSTEM =====\n"
             << "1. Add student record\n"
             << "2. Display all records\n"
             << "3. Save records to file\n"
             << "4. Load records from file\n"
             << "5. Find students above a total mark\n"
             << "6. Exit\n";
 
        choice = readInteger("Enter your choice: ");
 
        switch (choice) {
        case 1:
            addStudent(students);
            break;
        case 2:
            displayAll(students);
            break;
        case 3:
            saveToFile(students);
            break;
        case 4:
            loadFromFile(students);
            break;
        case 5: {
            int threshold = readInteger("Enter total-mark threshold (0-300): ");
            filterStudents(students, threshold);
            break;
        }
        case 6:
            cout << "Thank you for using the system.\n";
            break;
        default:
            cout << "Invalid menu choice. Please try again.\n";
        }
    } while (choice != 6);
 
    return 0;
}
