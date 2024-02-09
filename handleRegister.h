#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
int getNextStudentID() 
{
    int fd = open("student.txt", O_RDONLY);
    if (fd == -1) 
    {
        return 0; 
    }
    off_t offset = lseek(fd, 0, SEEK_END); 
    int numStudents = offset / sizeof(struct Student);
    close(fd);
    return numStudents; 
}
int getNextFacultyID() 
{
    int fd = open("faculty.txt", O_RDONLY);
    if (fd == -1) 
    {
        return 0; 
    }
    off_t offset = lseek(fd, 0, SEEK_END); 
    int numStudents = offset / sizeof(struct Faculty);
    close(fd);
    return numStudents; 
}
void writeFacultyToFile(int client_socket,const struct Faculty* fac) 
{
    int fd = open("faculty.txt", O_WRONLY | O_APPEND);
    if (fd == -1) 
    {
        perror("Error opening file");
        return;
    }
    struct flock lock;
    completeLock(&lock,fd);
	fcntl(fd,F_SETLKW,lock);
    if (write(fd, fac, sizeof(struct Faculty)) != sizeof(struct Faculty)) 
    {
        perror("Error writing to file");
        return;
    }
    unlock(&lock,fd);
    char *prompt="\nRegistered Successfully\n";
    send(client_socket,prompt,strlen(prompt),0);
    close(fd);
}
void registerFaculty(int client_socket)
{
    struct Faculty fac;
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompts[4]={"Enter Name : \n","Enter department : \n","Enter email : \n","Enter password : \n"};
    for(int k=0;k<4;k++) 
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
                fac.details[k][buffer_index] = '\0';  
                buffer_index = 0;
            } 
            else 
            {
                fac.details[k][buffer_index++] = buffer[i];
            }
        }
    }
    char id[5];
    int nextId=getNextFacultyID();
    if(nextId>=1000)
    {
        send(client_socket,"Maximum faculty limit reached.\nCannot Register\n",strlen("Maximum faculty limit reached.\nCannot Register\n"),0);
        return;
    }
    convertIndexToID(nextId,id);
    id[3]='\0';
    id[4]='\0';
    strncpy(fac.details[4],id,4);
    fac.coursesCount=0;
    for(int i=0;i<MAX_COURSES;i++)
    {
        fac.courses[i]=0;
    }
    id[3]='\n';
    id[4]='\0';
    send(client_socket,"Your LoginId : ",strlen("Your LoginId : "),0);
    send(client_socket,id,strlen(id),0);
    writeFacultyToFile(client_socket,&fac);
}
void writeStudentToFile(int client_socket,const struct Student* stu) 
{
    int fd = open("student.txt", O_WRONLY | O_APPEND);
    if (fd == -1) 
    {
        perror("Error opening file");
        return;
    }
    struct flock lock;
    completeLock(&lock,fd);
    if (write(fd, stu, sizeof(struct Student)) != sizeof(struct Student)) 
    {
        perror("Error writing to file");
    }
    char *prompt="\nRegisetred Successfully\n";
    send(client_socket,prompt,strlen(prompt),0);
    unlock(&lock,fd);
    close(fd);
}
void registerStudent(int client_socket)
{
    struct Student stu;
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompts[4]={"Enter Name : \n","Enter department : \n","Enter email : \n","Enter password : \n"};
    for(int k=0;k<4;k++) 
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
                stu.details[k][buffer_index] = '\0';  
                buffer_index = 0;
            } 
            else 
            {
                stu.details[k][buffer_index++] = buffer[i];
            }
        }
    }
    char id[5];
    int nextId=getNextStudentID();
    if(nextId>=1000)
    {
        send(client_socket,"Maximum students limit reached.\nCannot Register\n",strlen("Maximum students limit reached.\nCannot Register\n"),0);
        return;
    }
    convertIndexToID(nextId,id);
    id[3]='\0';
    id[4]='\0';
    strncpy(stu.details[4],id,4);
    stu.coursesCount=0;
    for(int i=0;i<MAX_COURSES;i++)
    {
        stu.courses[i]=0;
    }
    id[3]='\n';
    id[3]='\0';
    send(client_socket,"Your LoginId : ",strlen("Your LoginId : "),0);
    send(client_socket,id,strlen(id),0);
    writeStudentToFile(client_socket,&stu);
}

