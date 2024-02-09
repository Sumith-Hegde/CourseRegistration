#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
void adminCourses(int client_socket)
{
    int courses[MAX_COURSES];
    for(int i=0;i<MAX_COURSES;i++)
    {
        courses[i]=1;
    }
    viewCourses(courses,client_socket);
}
void handleAdminLoginFail(int client_socket)
{
    //notify(client_socket,"Login Fail\n");
    char *prompt="Login Fail\n";
    send(client_socket,prompt,strlen(prompt),0);
    return;
}
void handleAdminLoginSuccess(int client_socket)
{
    send(client_socket,"Login Successfull.....\n",strlen("Login Successfull.....\n"),0);
    char *prompt="What would you like to do??\nAdd Faculty(Enter 1)\nAdd Student(Enter 2)\nView Courses(Enter 3)\nLogOut(Enter 0)\n";
    int login=1;
    while(login)
    {
        char role=takeOption(prompt,client_socket);
        switch(role)
        {
            case '1':
                registerFaculty(client_socket);
                break;
            case '2':
                registerStudent(client_socket);
                break;
            case '3':
                adminCourses(client_socket);
                break;
            case '0':
                login=0;
                break;
            default:
                break;
        }
    }
}
