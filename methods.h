#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
#include <ctype.h>
#include <stdbool.h>
void unlock(struct flock *lock,int fd)
{
    lock->l_type=F_UNLCK;
	fcntl(fd,F_SETLKW,lock);
}
void completeLock(struct flock *lock,int fd)
{
	lock->l_type=F_WRLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=0;
	lock->l_len=0;
	lock->l_pid=getpid();
	fcntl(fd,F_SETLKW,lock);
}
void writeLockFaculty(struct flock *lock,off_t offset,int fd)
{
	lock->l_type=F_WRLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=offset;
	lock->l_len=sizeof(struct Faculty);
	lock->l_pid=getpid();
	fcntl(fd,F_SETLKW,lock);
}
void writeLockStudent(struct flock *lock,off_t offset,int fd)
{
	lock->l_type=F_WRLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=offset;
	lock->l_len=sizeof(struct Student);
	lock->l_pid=getpid();
	fcntl(fd,F_SETLKW,lock);
}
void writeLockCourse(struct flock *lock,off_t offset,int fd)
{
	lock->l_type=F_WRLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=offset;
	lock->l_len=sizeof(struct Course);
	lock->l_pid=getpid();
	fcntl(fd,F_SETLKW,lock);
}
void readLockFaculty(struct flock *lock,off_t offset,int fd)
{
	lock->l_type=F_RDLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=offset;
	lock->l_len=sizeof(struct Faculty);
	lock->l_pid=getpid();
	fcntl(fd,F_SETLKW,lock);
}
void readLockStudent(struct flock *lock,off_t offset,int fd)
{
	lock->l_type=F_RDLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=offset;
	lock->l_len=sizeof(struct Student);
	lock->l_pid=getpid();
	fcntl(fd,F_SETLKW,lock);
}
void readLockCourse(struct flock *lock,off_t offset,int fd)
{
	lock->l_type=F_RDLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=offset;
	lock->l_len=sizeof(struct Course);
	lock->l_pid=getpid();
	fcntl(fd,F_SETLKW,lock);
}
char* concatenateStrings(const char** strings, size_t numStrings) {
    // Calculate the total length of the concatenated string
    size_t totalLength = 0;
    for (size_t i = 0; i < numStrings; i++) {
        totalLength += strlen(strings[i]) + 1; // Add 1 for the '\n'
    }

    // Allocate memory for the concatenated string
    char* result = (char*)malloc(totalLength+1);
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    // Initialize the result string
    result[0] = '\0';

    // Concatenate the strings with '\n' between them
    for (size_t i = 0; i < numStrings; i++) {
        strcat(result, strings[i]);
        if (i < numStrings - 1) {
            strcat(result, "\n");
        }
    }
    // result[totalLength]='\n';
    return result;
}
void viewCourses(int *indices,int client_socket)
{
    int fd = open("courses.txt", O_RDONLY);
    if (fd == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0); 
        return;
    }
    struct Course temp;
    send(client_socket,"\nCourses :- \n",strlen("\nCourses :- \n"),0);
    for(int i=0;i<MAX_COURSES;i++)
    {
        if(indices[i]==0)
        {
            continue;
        }
        int index=i;
        off_t offset = index * sizeof(struct Course);
        struct flock lock;
        readLockCourse(&lock,offset,fd);
        if (lseek(fd, offset, SEEK_SET) == -1) 
        {
            send(client_socket,"Failed\n",strlen("Failed\n"),0);
            return ; 
        }
        if (read(fd, &temp, sizeof(struct Course)) != sizeof(struct Course)) 
        {
            return ; 
        }
        // for(int j=0;j<4;j++)
        // {
        //     char *prompt=temp.details[j];
        //     printf("%s\n",prompt);
        // }
        // send(client_socket,temp.details[0],strlen(temp.details[0]),0);
        // int totalLength=0;
        char *inId="-1";
        unlock(&lock,fd);
        if(strncmp(temp.details[3],inId,2)==0)
        {
            continue;
        }
        const char* strings[5];
        for (int j = 0; j < 4; j++) 
        {
            strings[j] = temp.details[j];
            // char prompt[50]="Name : ";
            // strcat(prompt,strings[j]);
            // send(client_socket,prompt,strlen(prompt),MSG_MORE);
        }
        strings[4]=temp.faculty;
        char *prompt1[5]={"Name : ", "Maximum Seats : ", "Credits : " , "CourseId : " , "Faculty : "};
        const char *strings1[10];
        int k=0;
        for(int i=0;i<10;i+=2)
        {
            strings1[i]=prompt1[k];
            k++;
        } 
        k=0;
        for(int i=1;i<10;i+=2)
        {
            strings1[i]=strings[k];
            k++;
        }
        char *prompt=concatenateStrings(strings1,10);
        // send(client_socket,prompt1,strlen(prompt1),0);
        send(client_socket,prompt,strlen(prompt),0);
        send(client_socket,"\n------------------------------------------\n",strlen("\n------------------------------------------\n"),0);
        // send(client_socket,"\n",strlen("\n"),0);
    }
    close(fd);
}

void updateFacultyToFile(int client_socket,struct Faculty* fac) 
{
    int fd = open("faculty.txt", O_WRONLY);
    if (fd == -1) 
    {
        perror("Error opening file");
        return;
    }
    int index=atoi(fac->details[4]);
    off_t offset = index * sizeof(struct Faculty);
    struct flock lock;
    writeLockFaculty(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    if (write(fd, fac, sizeof(struct Faculty)) != sizeof(struct Faculty)) 
    {
        perror("Error writing to file");
        return;
    }
    unlock(&lock,fd);
    char *prompt="Updated Successfully\n";
    send(client_socket,prompt,strlen(prompt),0);
    close(fd);
}
void updateStudentToFile(int client_socket,struct Student* stu) 
{
    int fd = open("student.txt", O_WRONLY);
    if (fd == -1) 
    {
        perror("Error opening file");
        return;
    }
    int index=atoi(stu->details[4]);
    off_t offset = index * sizeof(struct Student);
    struct flock lock;
    writeLockStudent(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    if (write(fd, stu, sizeof(struct Student)) != sizeof(struct Student)) 
    {
        perror("Error writing to file");
        return;
    }
    unlock(&lock,fd);
    char *prompt="Updated Successfully\n";
    send(client_socket,prompt,strlen(prompt),0);
    close(fd);
}
void updateCourseToFile(int client_socket,struct Course* co) 
{
    int fd = open("courses.txt", O_WRONLY);
    if (fd == -1) 
    {
        perror("Error opening file");
        return;
    }
    int index=atoi(co->details[3]);
    off_t offset = index * sizeof(struct Course);
    struct flock lock;
    writeLockCourse(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed update\n",strlen("Failed update\n"),0);
        return ; 
    }
    if (write(fd, co, sizeof(struct Course)) != sizeof(struct Course)) 
    {
        perror("Error writing to file");
        return;
    }
    unlock(&lock,fd);
    char *prompt="Updated Successfully\n";
    send(client_socket,prompt,strlen(prompt),0);
    close(fd);
}
void convertIndexToID(int index, char* id) 
{
        snprintf(id, 4, "%03d", index); 
}
bool isInteger(char *str) {
    if (str == NULL || str[0] == '\0'||str[0]=='\n') {
        return false;
    }

    int i = 0;

    while (1) 
    {
        if(str[i]=='\n'||str[i]=='\0'||str[i]=='\r')
        {
            return true;
        }
        if (!isdigit(str[i])) 
        {
            return false;  
        }
        i++;
    }

    return true;
}
char takeOption(char* prompts,int client_socket)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    for(int i=0;i<1;i++) 
    {
        send(client_socket,prompts,strlen(prompts),0);
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
    return command_buffer[0];
}
// void notify(int client_socket,char *prompt)
// {
//     send(client_socket,prompt,strlen(prompt),0);
//     return;
// }
