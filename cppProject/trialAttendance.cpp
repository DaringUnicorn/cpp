#include <iostream>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>  // Include this for std::vector
#include <sstream>  // Include this for std::istringstream
#include <iomanip>  // Include this for std::quoted
#include <functional>
#include "attendance.h"
#include <limits>

const std::string FILENAME = "students.txt";
#define MAX_COURSES 6

AttendanceManager* initializeAttendanceManager() {
    AttendanceManager* manager = new AttendanceManager;

    if (manager == nullptr) {
        std::cout << "Memory allocation failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize the students vector
    manager->students = std::vector<Student>();
    manager->studentCount = 0;

    // Initialize function pointers directly
    manager->addStudent = [manager](const Student& student) {
        manager->addStudentImpl(student);
    };
    manager->loadStudents = [manager]() {
        manager->loadStudentsImpl();
    };
    manager->deleteStudent = [manager](const Student& student) {
        manager->deleteStudentImpl(student);
    };

    return manager;
}




void freeAttendanceManager(AttendanceManager* manager) {
    if (manager == nullptr) {
        return;
    }

    // Clear the students vector
    manager->students.clear();

    // Delete the manager object
    delete manager;
}



void freeAttendanceAnalysis(AttendanceAnalysis* analysis) {
    if (analysis != nullptr) {
        delete analysis;
    }
}


int findNumberOfStudentsInCourse(CourseType course) {
    std::ifstream file(FILENAME.c_str());
    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        exit(EXIT_FAILURE);
    }

    int id, count = 0;
    std::string name;
    int courseNumber, attendanceCount;

    while (file >> id >> std::quoted(name) >> courseNumber >> attendanceCount) {
        if (courseNumber == static_cast<int>(course)) {
            count++;
        }
    }

    file.close();
    return count;
}



void AttendanceManager::summarizeAttendance() {
    if (students.empty()) {
        std::cout << "No students loaded." << std::endl;
        return;
    }

    int totalAttendance = 0;
    for (const auto& student : students) {
        totalAttendance += student.attendance;
    }

    std::cout << "Total attendance for all students: " << totalAttendance << std::endl;
}


void listStudents() {
    std::ifstream file("students.txt");
    if (file.is_open()) {
        std::cout << "---- Student List ----\n";
        std::cout << "ID\tName\tCourse\tAttendance\n";

        while (file.peek() != EOF) {
            int studentID, course, attendanceCount;
            std::string studentName;

            file >> studentID;
            file.ignore(); // Ignore the delimiter
            std::getline(file, studentName, ';');
            file >> course;
            file.ignore(); // Ignore the delimiter
            file >> attendanceCount;
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore the rest of the line

            std::cout << studentID << '\t' << studentName << '\t' << course << '\t' << attendanceCount << '\n';
        }

        file.close();
    } else {
        std::cout << "File opening error\n";
    }
}


#include <fstream>
#include <sstream>

int checkExistingID(int studentID) {
    std::ifstream file("students.txt");
    if (file.is_open()) {
        int id;
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            if (iss >> id) {
                if (id == studentID) {
                    file.close();
                    return 1; // ID mevcut
                }
            }
        }
        file.close();
    }
    return 0; // ID mevcut değil
}


void addStudentToFile(int studentID, const std::string& name, CourseType course, int attendanceCount) {
    std::ofstream file(FILENAME, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        exit(EXIT_FAILURE);
    }

    file << studentID << ';' << name << ';' << static_cast<int>(course) << ';' << attendanceCount << '\n';
}


Student* loadStudentsFromFile(int& studentCount) {
    std::ifstream file(FILENAME.c_str());
    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        exit(EXIT_FAILURE);
    }

    Student* students = nullptr;
    studentCount = 0;
    int id, course;
    std::string name;

    while (file >> id >> name >> course) {
        studentCount++;
        Student* temp = reinterpret_cast<Student*>(realloc(students, studentCount * sizeof(Student)));
        if (temp == nullptr) {
            std::cerr << "Memory allocation failed." << std::endl;
            free(students);
            exit(EXIT_FAILURE);
        }
        else {
            students = temp;
        }

        students[studentCount - 1] = Student(id, name, static_cast<CourseType>(course), 0);

        if (course < 0 || course >= MAX_COURSES) {
            std::cerr << "Invalid course type for student ID " << id << ". Skipping..." << std::endl;
            continue;
        }
    }

    file.close();

    return students;
}



void AttendanceManager::updateAttendance(int studentID, int newAttendanceCount) {
    std::ifstream file(FILENAME.c_str());
    std::ofstream tempFile("temp.txt");
    int id;
    std::string name;
    int course;
    int attendance;
    bool found = false;

    if (!file.is_open() || !tempFile.is_open()) {
        std::cout << "File opening error!" << std::endl;
        return;
    }

    while (file >> id >> std::quoted(name) >> course >> attendance) {
        if (id == studentID) {
            found = true;
            students.push_back(Student(id, name, static_cast<CourseType>(course), newAttendanceCount));
            tempFile << id << ';' << name << ';' << course << ';' << newAttendanceCount << '\n';
        } else {
            tempFile << id << ';' << name << ';' << course << ';' << attendance << '\n';
        }
    }

    file.close();
    tempFile.close();

    std::remove(FILENAME.c_str());
    std::rename("temp.txt", FILENAME.c_str());

    if (!found) {
        std::cout << "Student could not be found!" << std::endl;
    } else {
        std::cout << "Attendance number updated." << std::endl;
    }
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>  // std::remove ve std::rename için

void deleteStudentFromFile(int studentID, AttendanceManager* manager) {
    std::ifstream inputFile(FILENAME);
    std::ofstream tempFile("temp.txt");

    if (!inputFile.is_open() || !tempFile.is_open()) {
        std::cerr << "File opening error!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        int id;
        std::string name;
        int course;
        int attendance;

        if (iss >> id >> std::quoted(name) >> course >> attendance) {
            if (id != studentID) {
                manager->addStudentToFile(id, name, static_cast<CourseType>(course), attendance);
            }
        } else {
            std::cerr << "Invalid format: " << line << std::endl;
        }
    }

    inputFile.close();
    tempFile.close();

    // Dosyayı silme işlemi
    if (std::remove(FILENAME.c_str()) != 0) {
        std::cerr << "Could not delete!" << std::endl;
        return;
    }

    // Dosyayı yeniden adlandırma işlemi
    if (std::rename("temp.txt", FILENAME.c_str()) != 0) {
        std::cerr << "Rename failed!" << std::endl;
        return;
    }

    std::cout << "Student deleted!" << std::endl;
}


// void AttendanceManager::addStudent(const Student& student) {
//     students.push_back(student);
//     studentCount++;
// }

void addStudentInteractive(AttendanceManager* manager) {
    int studentID;
    std::string name;
    int course;
    int attendance;

    std::cout << "Enter student ID: ";
    std::cin >> studentID;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear unnecessary newline character

    if (manager->checkExistingID(studentID) != 0) {
        std::cout << "Existing ID." << std::endl;
    } else {
        std::cout << "Enter student name: ";
        std::getline(std::cin, name);

        std::cout << "Enter course (COMPUTER_SCIENCE - 0, ELECTRICAL_ENGINEERING - 1): ";
        std::cin >> course;

        if (course < 0 || course > 1) {
            std::cout << "Invalid course selection." << std::endl;
        } else {
            std::cout << "Enter attendance: ";
            std::cin >> attendance;

            if (attendance < 0) {
                std::cout << "Please enter a valid attendance value." << std::endl;
            } else {
                // Öğrenci nesnesini oluştur ve ardından ekleyebilirsin
                Student newStudent(studentID, name, static_cast<CourseType>(course), attendance);
                manager->addStudent(newStudent);
                std::cout << "Student added successfully." << std::endl;
            }
        }
    }
}

Student* findStudentByID(int studentID) {
    std::ifstream file(FILENAME.c_str());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            int id, course, attendance;
            char name[50];
            sscanf(line.c_str(), "%d;%49[^;];%d;%d", &id, name, &course, &attendance);

            if (id == studentID) {
                Student* foundStudent = new Student(id, std::string(name), static_cast<CourseType>(course), attendance);
                if (foundStudent != nullptr) {
                    file.close();
                    return foundStudent;
                } else {
                    std::cout << "Memory allocation error!" << std::endl;
                }
            }
        }
        file.close();
    } else {
        std::cout << "File opening error!" << std::endl;
    }
    return nullptr;
}


void AttendanceManager::checkAttendance(int thresholdAttendance) {
    std::ofstream tempFile("temp.txt");
    std::ifstream file(FILENAME.c_str());

    if (!file || !tempFile) {
        std::cout << "File opening error!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        int id;
        std::string name;
        int course;
        int attendance;
        std::istringstream iss(line);

        if (iss >> id >> std::quoted(name) >> course >> attendance) {
            Student student(id, name, static_cast<CourseType>(course), attendance);

            if (attendance < thresholdAttendance) {
                tempFile << student.id << ';' << std::quoted(student.name) << ';' << static_cast<int>(student.course) << ';' << student.attendance << '\n';
            }
        } else {
            std::cout << "Invalid format: " << line << std::endl;
        }
    }

    file.close();
    tempFile.close();

    if (std::remove(FILENAME.c_str()) != 0) {
        std::cout << "Could not delete!" << std::endl;
        return;
    }

    if (std::rename("temp.txt", FILENAME.c_str()) != 0) {
        std::cout << "Rename failed!" << std::endl;
        return;
    }

    std::cout << "Students deleted exceeding attendance limit." << std::endl;
}


void listStudentsByCourse(AttendanceManager* manager) {
    std::cout << "Enter course type (COMPUTER_SCIENCE - 0, ELECTRICAL_ENGINEERING - 1): ";
    int courseChoice;
    std::cin >> courseChoice;
    CourseType course = static_cast<CourseType>(courseChoice);

    std::cout << "---- " << ((course == COMPUTER_SCIENCE) ? "COMPUTER_SCIENCE" : "ELECTRICAL_ENGINEERING") << " Course Student List ----\n";
    std::cout << "ID\tName\tCourse\tAttendance\n";

    for (const auto& student : manager->students) {
        if (student.course == course) {
            std::cout << student.id << "\t" << student.name << "\t" << ((student.course == COMPUTER_SCIENCE) ? "COMPUTER_SCIENCE" : "ELECTRICAL_ENGINEERING") << "\t" << student.attendance << "\n";
        }
    }
}
Student* getStudentByID(AttendanceManager* manager, int studentID) {
    for (auto& student : manager->students) {
        if (student.id == studentID) {
            return &student;
        }
    }
    return nullptr;
}
