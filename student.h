#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
void enrollCourse(int client_socket,struct Student *stu)
{
    int courses[MAX_COURSES];
    for(int i=0;i<MAX_COURSES;i++)
    {
        courses[i]=1;
    }
    viewCourses(courses,client_socket);
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompt="\nEnter ID of Course you want to enroll\nExit(enter -1)";
    int courseId=-1;
    while(1)
    {
        for(int k=0;k<1;k++) 
        {
            send(client_socket,prompt,strlen(prompt),0);
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) 
            {
                break;
            }
            for (int i = 0; i < bytes_received; i++) 
            {
                if (buffer[i] == '\n') 
                {
                    command_buffer[buffer_index] = '\0';  
                    buffer_index = 0;
                } 
                else 
                {
                    command_buffer[buffer_index++] = buffer[i];
                }
            }
        }
        if(atoi(command_buffer)==-1)
        {
            break;
        }
        if(!isInteger(command_buffer))
        {
            char *msg="Invalid ID\n";
            send(client_socket,msg,strlen(msg),0);
        }
        else
        {
            courseId=atoi(command_buffer);
            break;
        }
    }
    int fd = open("courses.txt", O_RDONLY);
    if (fd == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0); 
        return;
    }
    struct Course temp;
    off_t offset = courseId * sizeof(struct Course);
    struct flock lock;
    readLockCourse(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    if (read(fd, &temp, sizeof(struct Course)) != sizeof(struct Course)) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    unlock(&lock,fd);
    close(fd);
    if(stu->courses[courseId]==1)
    {
        send(client_socket,"You have already enrolled\n",strlen("You have already enrolled\n"),0);
        return;
    }
    if(temp.currentStudentsCount==atoi(temp.details[1]))
    {
        send(client_socket,"MAX students limit reached,cannot enroll\n",strlen("MAX students limit reached,cannot enroll\n"),0);
        return;
    }
    else
    {
        stu->courses[courseId]=1;
        temp.students[temp.currentStudentsIndex]=atoi(stu->details[4]);
        temp.currentStudentsCount++;
        temp.currentStudentsIndex++;
        updateCourseToFile(client_socket,&temp);
        updateStudentToFile(client_socket,stu);
    }
}
void deenrollCourse(int client_socket,struct Student *stu)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompt="Enter ID of Course you want to de-enroll\n";
    int courseId=-1;
    while(1)
    {
        for(int k=0;k<1;k++) 
        {
            send(client_socket,prompt,strlen(prompt),0);
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) 
            {
                break;
            }
            for (int i = 0; i < bytes_received; i++) 
            {
                if (buffer[i] == '\n') 
                {
                    command_buffer[buffer_index] = '\0';  
                    buffer_index = 0;
                } 
                else 
                {
                    command_buffer[buffer_index++] = buffer[i];
                }
            }
        }
        if(!isInteger(command_buffer))
        {
            char *msg="Invalid ID\n";
            send(client_socket,msg,strlen(msg),0);
        }
        else
        {
            courseId=atoi(command_buffer);
            break;
        }
    }
    if(stu->courses[courseId]==0)
    {
        send(client_socket,"You have not enrolled to this course\n",strlen("You have not enrolled to this course\n"),0);
        return;
    }
    stu->courses[courseId]=0;
    int fd = open("courses.txt", O_RDONLY);
    if (fd == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0); 
        return;
    }
    struct Course temp;
    off_t offset = courseId * sizeof(struct Course);
    struct flock lock;
    readLockCourse(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    if (read(fd, &temp, sizeof(struct Course)) != sizeof(struct Course)) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    unlock(&lock,fd);
    close(fd);
    for(int i=0;i<atoi(temp.details[1]);i++)
    {
        if(temp.students[i]==atoi(stu->details[4]))
        {
            temp.currentStudentsCount--;
            temp.students[i]=-1;
            break;
        }
    }
    updateCourseToFile(client_socket,&temp);
    updateStudentToFile(client_socket,stu);
}
void changePasswordStudent(int client_socket,struct Student *stu)
{
    char buffer[1024];
    char command_buffer[1024];
    char passwords[2][1024];
    int buffer_index = 0;
    char *prompts[2]={"\nEnter New Password\n","Confirm New Password\n"};
    while(1)
    {
        for(int k=0;k<2;k++) 
        {
            send(client_socket,prompts[k],strlen(prompts[k]),0);
            int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
            if (bytes_received <= 0) 
            {
                break;
            }
            for (int i = 0; i < bytes_received; i++) 
            {
                if (buffer[i] == '\n') 
                {
                    passwords[k][buffer_index] = '\0';  
                    buffer_index = 0;
                } 
                else 
                {
                    passwords[k][buffer_index++] = buffer[i];
                }
            }
        }
        if(strcmp(passwords[0],passwords[1])==0)
        {
            strcpy(stu->details[3],passwords[0]);
            int fd = open("student.txt", O_RDWR);
            int index = atoi(stu->details[4]);
            off_t offset = index * sizeof(struct Student);
            struct flock lock;
            readLockStudent(&lock,offset,fd);
            if (fd == -1||lseek(fd, offset, SEEK_SET) == -1) 
            {
                char *prompt="Failed\n";
                send(client_socket,prompt,strlen(prompt),0);
                break;
            }
            struct Student temp;
            char *prompt;
            if (read(fd, &temp, sizeof(struct Student)) == sizeof(struct Student)) 
            {
                if (strcmp(temp.details[4], stu->details[4]) == 0) 
                {
                    lseek(fd, offset, SEEK_SET);
                    if(write(fd, stu, sizeof(struct Student))==sizeof(struct Student))
                    {
                        prompt="Password changed Succefully\n";
                    }
                    else
                    {
                        prompt="Failed\n";
                    }
                }
                else
                {
                    prompt="Invaild name\n";
                }
            }
            else
            {
                prompt="Failed\n";
            }
            send(client_socket,prompt,strlen(prompt),0);
            unlock(&lock,fd);
            close(fd);
            break;
        }
        else
        {
            char *prompt="Passwords do not match\n";
            send(client_socket,prompt,strlen(prompt),0);
        }
    }
}
void viewRegisteredCourses(int client_socket,struct Student *stu)
{
    viewCourses(stu->courses,client_socket);
}
void modifyDetails()
{
    printf("Modify details\n");
}
void handleStudentLoginSuccess(int client_socket,struct Student *stu)
{
    send(client_socket,"\nLogin Successfull.....\n",strlen("\nLogin Successfull.....\n"),0);
    char *prompt="\nWhat would you like to do??\nEnroll to a Course(Enter 1)\nDe-enroll to a Course(Enter 2)\nChange Password(Enter 3)\nView registered Courses(Enter 4)\nModify your details(Enter 5)\nLogOut(Enter 0)\n";
    int login=1;
    while(login)
    {
        char role=takeOption(prompt,client_socket);
        switch(role)
        {
            case '1':
                enrollCourse(client_socket,stu);
                break;
            case '2':
                deenrollCourse(client_socket,stu);
                break;
            case '3':
                changePasswordStudent(client_socket,stu);
                break;
            case '4':
                viewRegisteredCourses(client_socket,stu);
                break;
            case '5':
                modifyDetails();
                break;
            case '0':
                login=0;
                break;
            default:
                break;
        }
    }
}
void handleStudentLoginFail(int client_socket)
{
    //notify(client_socket,"Login Fail\n");
    char *prompt="Login Fail\n";
    send(client_socket,prompt,strlen(prompt),0);
    return;
}
