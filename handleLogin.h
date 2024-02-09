#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
int readFacultyByIndex(struct Faculty* fac,int index) 
{
    int fd = open("faculty.txt", O_RDONLY);
    if (fd == -1) 
    {
        return 0; 
    }
    off_t offset = index * sizeof(struct Faculty);
    struct flock lock;
    readLockFaculty(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        close(fd);
        return 0; 
    }

    if (read(fd, fac, sizeof(struct Faculty)) != sizeof(struct Faculty)) 
    {
        close(fd);
        return 0; 
    }
    unlock(&lock,fd);
    close(fd);
    return 1;  
}

int authenticateFaculty(const char* username, const char* password,struct Faculty *fac) 
{
    char indexStr[4];
    strncpy(indexStr, username, 3);
    indexStr[3] = '\0';
    int index = atoi(indexStr);
    if (!readFacultyByIndex(fac,index)) 
    {
        return 2;
    }
    if (strncmp((fac->details)[4], username,3) == 0) 
    {
        if (strcmp((fac->details)[3], password) == 0) 
        {
            return 1; 
        } 
        else 
        {
            return 0;
        }
    }
    return 2; 
}
int loginFaculty(int client_socket,struct Faculty *fac)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char details[2][50];
    char *prompts[2]={"Enter username : \n","Enter password : \n"};
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
                details[k][buffer_index] = '\0';  
                buffer_index = 0;
            } 
            else 
            {
                details[k][buffer_index++] = buffer[i];
            }
        }
    }
    if(strlen(details[0])!=4)
    {
        return 0;
    }
    int retVal=authenticateFaculty(details[0],details[1],fac);
    return retVal;
}
int readStudentByIndex(struct Student* stu,int index) 
{
    int fd = open("student.txt", O_RDONLY);
    if (fd == -1) 
    {
        return 0; 
    }
    off_t offset = index * sizeof(struct Student);
    struct flock lock;
    readLockStudent(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        close(fd);
        return 0; 
    }

    if (read(fd, stu, sizeof(struct Student)) != sizeof(struct Student)) 
    {
        close(fd);
        return 0; 
    }
    unlock(&lock,fd);
    close(fd);
    return 1;  
}

int authenticateStudent(const char* username, const char* password,struct Student *stu) 
{
    char indexStr[4];
    strncpy(indexStr, username, 3);
    indexStr[3] = '\0';
    int index = atoi(indexStr);
    if (!readStudentByIndex(stu,index)) 
    {
        return 2;
    }
    if (strncmp(stu->details[4], username,3) == 0) 
    {
        if (strcmp(stu->details[3], password) == 0) 
        {
            return 1; 
        } 
        else 
        {
            return 0;
        }
    }
    return 2; 
}
int loginStudent(int client_socket,struct Student *stu)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char details[2][50];
    char *prompts[2]={"Enter username : \n","Enter password : \n"};
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
                details[k][buffer_index] = '\0';  
                buffer_index = 0;
            } 
            else 
            {
                details[k][buffer_index++] = buffer[i];
            }
        }
    }
    if(strlen(details[0])!=4)
    {
        return 0;
    }
    int retVal=authenticateStudent(details[0],details[1],stu);
    return retVal;
}

