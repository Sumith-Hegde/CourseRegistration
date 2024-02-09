#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 50
#define MAX_COURSES 150
#define MAX_STUDENTS 300
#define COURSES_ENROLL 7
struct Course
{
    // char name[MAX];
    // char maxSeats[MAX];
    // char credits[MAX];
    // 0-name
    // 1-maxSeats
    // 2-credits
    // 3-courseId
    char details[4][MAX];
    int currentStudentsCount;
    int currentStudentsIndex;
    int students[MAX_STUDENTS];
    char faculty[MAX];
};
struct Admin
{
    char username[MAX];
    char password[MAX];
};
struct Faculty
{
    // char name[MAX];
    // char department[MAX];
    // char email[MAX];
    // char password[MAX];
    // char facultyId[MAX];
    // 0-name
    // 1-department
    // 2-email
    // 3-password
    // 4-facultyId
    char details[5][MAX];
    int courses[MAX_COURSES];
    int coursesCount;
};
struct Student
{
    // char name[MAX];
    // char department[MAX];
    // char email[MAX];
    // char studentId[MAX];
    // char password[MAX];
    // 0-name
    // 1-department
    // 2-email
    // 3-password
    // 4-studentId
    char details[5][MAX];
    int coursesCount;
    int courses[MAX_COURSES];
};
