#include <iostream>
#include "trialAttendance.cpp"
#include "attendance.h"
int main(int argc, char const *argv[])
{
    testOperations();
    return 0;
}


int testOperations() {
    AttendanceManager* manager = initializeAttendanceManager();

    int choice;
    Student* studentToRemove;  // Move the declaration outside the switch statement

    do {
        std::cout << "\n--- Menu ---\n";
        std::cout << "1- Add student\n";
        std::cout << "2- Remove student\n";
        std::cout << "3- List students\n";
        std::cout << "4- Find student by ID\n";
        std::cout << "5- Delete students exceeding attendance limit\n";
        std::cout << "6- Find the number of students in a course\n";
        std::cout << "7- Fetch students by course\n";
        std::cout << "0- Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                addStudentInteractive(manager);
                break;

            case 2:
                int studentId;
                std::cout << "Enter student ID to remove: ";
                std::cin >> studentId;
                studentToRemove = getStudentByID(manager, studentId);  // Move it here
                if (studentToRemove != nullptr) {
                    manager->deleteStudent(*studentToRemove);
                    std::cout << "Student removed successfully.\n";
                } else {
                    std::cout << "Student not found with ID " << studentId << ".\n";
                }
                break;

            case 3:
                listStudents();
                break;

            case 4: {
                int studentID;
                std::cout << "Enter student ID to find: ";
                std::cin >> studentID;
                Student* foundStudent = getStudentByID(manager, studentID);
                if (foundStudent != nullptr) {
                    std::cout << "Student found:\n";
                    std::cout << "ID: " << foundStudent->id << ", Name: " << foundStudent->name
                              << ", Course: " << foundStudent->course << ", Attendance: " << foundStudent->attendance << "\n";
                } else {
                    std::cout << "Student not found with ID " << studentID << ".\n";
                }
                break;
            }

            case 5: {
                int limit;
                std::cout << "Enter attendance limit: ";
                std::cin >> limit;
                // Assuming checkAttendance is defined somewhere
                checkAttendance(limit);
                break;
            }

            case 6: {
                int courseChoice;
                std::cout << "Enter course type (COMPUTER_SCIENCE - 0, ELECTRICAL_ENGINEERING - 1): ";
                std::cin >> courseChoice;
                CourseType course = static_cast<CourseType>(courseChoice);

                int numberOfStudents = manager->findNumberOfStudentsInCourse(course);
                std::cout << "Number of students in the selected course: " << numberOfStudents << "\n";
                break;
            }

            case 7:
                listStudentsByCourse(manager);
                break;

            case 0:
                std::cout << "Exiting...\n";
                // Assuming CLEAR_CONSOLE is defined somewhere
                CLEAR_CONSOLE();
                break;

            default:
                std::cout << "Invalid choice. Please enter a valid option.\n";
                break;
        }
    } while (choice != 0);

    freeAttendanceManager(manager);
    return 0;
}

