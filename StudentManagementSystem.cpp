#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

// Custom exception class for student-related errors
class StudentException : public exception {
private:
    string message;
public:
    StudentException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// ========== OOP: Student Class ==========
class Student {
private:
    string name;
    int rollNo;
    float marks;

public:
    // Constructor
    Student(string n = "", int r = 0, float m = 0.0f) : name(n), rollNo(r), marks(m) {
        // Validate data during construction
        if (rollNo < 0) throw StudentException("Roll number cannot be negative");
        if (marks < 0 || marks > 100) throw StudentException("Marks must be between 0 and 100");
    }

    // Getter functions
    string getName() const { return name; }
    int getRollNo() const { return rollNo; }
    float getMarks() const { return marks; }

    // Setter functions with validation
    void setName(string n) { 
        if (n.empty()) throw StudentException("Name cannot be empty");
        name = n; 
    }
    
    void setRollNo(int r) { 
        if (r < 0) throw StudentException("Roll number cannot be negative");
        rollNo = r; 
    }
    
    void setMarks(float m) { 
        if (m < 0 || m > 100) throw StudentException("Marks must be between 0 and 100");
        marks = m; 
    }

    // Display student details
    void display() const {
        cout << left << setw(20) << name << setw(10) << rollNo << setw(10) << marks << endl;
    }

    // For file writing
    friend ofstream& operator<<(ofstream& ofs, const Student& s);
    // For file reading
    friend ifstream& operator>>(ifstream& ifs, Student& s);
};

// File Handling: Save student to file
ofstream& operator<<(ofstream& ofs, const Student& s) {
    ofs << s.name << endl;
    ofs << s.rollNo << endl;
    ofs << s.marks << endl;
    return ofs;
}

// File Handling: Load student from file
ifstream& operator>>(ifstream& ifs, Student& s) {
    try {
        getline(ifs, s.name);
        if (ifs.fail()) throw StudentException("Failed to read name from file");
        
        ifs >> s.rollNo;
        if (ifs.fail()) throw StudentException("Failed to read roll number from file");
        
        ifs >> s.marks;
        if (ifs.fail()) throw StudentException("Failed to read marks from file");
        
        ifs.ignore(); // To skip the newline character
    }
    catch (...) {
        ifs.close();
        throw StudentException("Corrupted data in file");
    }
    return ifs;
}

// ========== OOP: Management System Class ==========
class StudentManagementSystem {
private:
    vector<Student> students;

    // File Handling: Load data from file with exception handling
    void loadFromFile() {
        ifstream file("students.dat");
        if (!file) {
            cout << "No existing data file found. Starting fresh." << endl;
            return;
        }

        try {
            Student s;
            while (file >> s) {
                students.push_back(s);
            }
            if (file.bad()) {
                throw StudentException("Error reading from file");
            }
            cout << "Data loaded successfully. " << students.size() << " records found." << endl;
        }
        catch (const StudentException& e) {
            cout << "Warning: " << e.what() << ". Starting with empty database." << endl;
            students.clear();
        }
        file.close();
    }

    // File Handling: Save data to file with exception handling
    void saveToFile() {
        ofstream file("students.dat");
        if (!file) {
            throw StudentException("Cannot create data file");
        }

        try {
            for (const Student& s : students) {
                file << s;
                if (file.fail()) {
                    throw StudentException("Failed to write student data to file");
                }
            }
            cout << "Data saved successfully. " << students.size() << " records stored." << endl;
        }
        catch (const StudentException& e) {
            file.close();
            throw StudentException(string("Save failed: ") + e.what());
        }
        file.close();
    }

    // Find student by roll number
    int findStudentIndex(int rollNo) {
        for (int i = 0; i < students.size(); i++) {
            if (students[i].getRollNo() == rollNo) {
                return i;
            }
        }
        return -1;
    }

    // Utility function to clear invalid input
    void clearCin() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

public:
    // Constructor
    StudentManagementSystem() {
        try {
            loadFromFile();
        }
        catch (const exception& e) {
            cout << "Initialization error: " << e.what() << endl;
        }
    }

    // Destructor
    ~StudentManagementSystem() {
        try {
            saveToFile();
        }
        catch (const exception& e) {
            cout << "Shutdown error: " << e.what() << endl;
        }
    }

    // Add new student with comprehensive input validation
    void addStudent() {
        try {
            string name;
            int rollNo;
            float marks;

            cout << "\nEnter Student Details:\n";
            cout << "Name: ";
            cin.ignore();
            getline(cin, name);
            if (name.empty()) throw StudentException("Name cannot be empty");

            cout << "Roll No: ";
            if (!(cin >> rollNo)) {
                clearCin();
                throw StudentException("Invalid input for roll number");
            }
            if (rollNo < 0) throw StudentException("Roll number cannot be negative");

            cout << "Marks: ";
            if (!(cin >> marks)) {
                clearCin();
                throw StudentException("Invalid input for marks");
            }
            if (marks < 0 || marks > 100) throw StudentException("Marks must be between 0-100");

            if (findStudentIndex(rollNo) != -1) {
                throw StudentException("Student with this Roll No already exists");
            }

            Student newStudent(name, rollNo, marks);
            students.push_back(newStudent);
            cout << "Student added successfully!" << endl;
        }
        catch (const StudentException& e) {
            cout << "Error: " << e.what() << endl;
        }
        catch (...) {
            cout << "Unexpected error occurred while adding student" << endl;
        }
    }

    // Display all students
    void displayAll() {
        if (students.empty()) {
            cout << "No students found!" << endl;
            return;
        }

        cout << "\n" << left << setw(20) << "Name" << setw(10) << "Roll No" << setw(10) << "Marks" << endl;
        cout << "----------------------------------------" << endl;
        for (const Student& s : students) {
            s.display();
        }
    }

    // Search student by roll number with input validation
    void searchStudent() {
        try {
            int rollNo;
            cout << "Enter Roll No to search: ";
            if (!(cin >> rollNo)) {
                clearCin();
                throw StudentException("Invalid input for roll number");
            }

            int index = findStudentIndex(rollNo);
            if (index == -1) {
                throw StudentException("Student not found");
            }

            cout << "\nStudent Found:" << endl;
            cout << left << setw(20) << "Name" << setw(10) << "Roll No" << setw(10) << "Marks" << endl;
            cout << "----------------------------------------" << endl;
            students[index].display();
        }
        catch (const StudentException& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    // Update student details with validation
    void updateStudent() {
        try {
            int rollNo;
            cout << "Enter Roll No to update: ";
            if (!(cin >> rollNo)) {
                clearCin();
                throw StudentException("Invalid input for roll number");
            }

            int index = findStudentIndex(rollNo);
            if (index == -1) {
                throw StudentException("Student not found");
            }

            string name;
            float marks;

            cout << "Enter new Name: ";
            cin.ignore();
            getline(cin, name);
            if (name.empty()) throw StudentException("Name cannot be empty");

            cout << "Enter new Marks: ";
            if (!(cin >> marks)) {
                clearCin();
                throw StudentException("Invalid input for marks");
            }
            if (marks < 0 || marks > 100) throw StudentException("Marks must be between 0-100");

            students[index].setName(name);
            students[index].setMarks(marks);
            cout << "Student details updated successfully!" << endl;
        }
        catch (const StudentException& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    // Delete student with validation
    void deleteStudent() {
        try {
            int rollNo;
            cout << "Enter Roll No to delete: ";
            if (!(cin >> rollNo)) {
                clearCin();
                throw StudentException("Invalid input for roll number");
            }

            int index = findStudentIndex(rollNo);
            if (index == -1) {
                throw StudentException("Student not found");
            }

            students.erase(students.begin() + index);
            cout << "Student deleted successfully!" << endl;
        }
        catch (const StudentException& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    // Show statistics
    void showStatistics() {
        if (students.empty()) {
            cout << "No students found!" << endl;
            return;
        }

        float total = 0, maxMarks = students[0].getMarks(), minMarks = students[0].getMarks();
        for (const Student& s : students) {
            total += s.getMarks();
            if (s.getMarks() > maxMarks) maxMarks = s.getMarks();
            if (s.getMarks() < minMarks) minMarks = s.getMarks();
        }

        cout << "\n--- Statistics ---" << endl;
        cout << "Total Students: " << students.size() << endl;
        cout << "Average Marks: " << fixed << setprecision(2) << (total / students.size()) << endl;
        cout << "Highest Marks: " << maxMarks << endl;
        cout << "Lowest Marks: " << minMarks << endl;
    }
};

// ========== Main Function ==========
int main() {
    StudentManagementSystem sms;
    int choice;

    cout << "=== STUDENT MANAGEMENT SYSTEM ===" << endl;

    do {
        cout << "\nMain Menu:" << endl;
        cout << "1. Add Student" << endl;
        cout << "2. Display All Students" << endl;
        cout << "3. Search Student" << endl;
        cout << "4. Update Student" << endl;
        cout << "5. Delete Student" << endl;
        cout << "6. Show Statistics" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter your choice (1-7): ";

        try {
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw StudentException("Invalid menu choice");
            }

            switch (choice) {
                case 1: sms.addStudent(); break;
                case 2: sms.displayAll(); break;
                case 3: sms.searchStudent(); break;
                case 4: sms.updateStudent(); break;
                case 5: sms.deleteStudent(); break;
                case 6: sms.showStatistics(); break;
                case 7: cout << "Exiting... Thank you for using the system!" << endl; break;
                default: cout << "Invalid choice! Please enter 1-7." << endl;
            }
        }
        catch (const exception& e) {
            cout << "System Error: " << e.what() << endl;
        }
    } while (choice != 7);

    return 0;
}
