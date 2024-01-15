#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

#define CLEAR_CONSOLE() system("cls")

enum CourseType {
    COMPUTER_SCIENCE,
    ELECTRICAL_ENGINEERING
};

class Student {
public:
    int id;
    std::string name;
    CourseType course;
    int attendance;

    Student(int id, std::string name, CourseType course, int attendance)
        : id(id), name(name), course(course), attendance(attendance) {}
};

class AttendanceManager {
public:
    
    std::vector<Student> students;
    int studentCount;
    
    std::function<void(const Student&)> addStudent = [this](const Student& student) {
        this->addStudentImpl(student);
    };

    std::function<void()> loadStudents = [this]() {
        this->loadStudentsImpl();
    };

    std::function<void(const Student&)> deleteStudent = [this](const Student& student) {
        this->deleteStudentImpl(student);
    };
    
    void addStudentToFile(int studentID, const std::string& name, CourseType course, int attendanceCount);
    void updateAttendance(int studentID, int newAttendanceCount);
    void listStudentsByCourse();
    void summarizeAttendance();
    int findNumberOfStudentsInCourse(CourseType course);

    static void addStudentStatic(AttendanceManager* manager, const Student& student);
    static void loadStudentsStatic(AttendanceManager* manager);
    static void deleteStudentStatic(AttendanceManager* manager, const Student& student);
    
    int checkExistingID(int studentID);
    void checkAttendance(int thresholdAttendance);
    
    void addStudentImpl(const Student& student);
    void loadStudentsImpl();
    void deleteStudentImpl(const Student& student);
};

class AttendanceAnalysis {
public:
    int findNumberOfStudentsInCourse(CourseType course);
    void summarizeAttendance(AttendanceManager& manager);
};

AttendanceManager* initializeAttendanceManager();
AttendanceAnalysis* initializeAttendanceAnalysis();

void freeAttendanceManager(AttendanceManager* manager);
void freeAttendanceAnalysis(AttendanceAnalysis* analysis);

int findNumberOfStudentsInCourse(CourseType course);
void summarizeAttendance(AttendanceManager* manager);
void listStudents();

int checkExistingID(int studentID);
void addStudentToFile(int studentID, const std::string& name, CourseType course, int attendance);
std::vector<Student> loadStudentsFromFile(int* studentCount);
void deleteStudentFromFile(int studentID);
void addStudentInteractive(AttendanceManager* manager);
Student* getStudentByID(AttendanceManager* manager, int studentID);
Student* findStudentByID(int studentID);
void checkAttendance(int thresholdAttendance);
int testOperations();
void listStudentsByCourse(AttendanceManager* manager);

#endif /* ATTENDANCE_H */
