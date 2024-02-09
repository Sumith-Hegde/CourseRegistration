#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
void removeStudentFromCourse(int client_socket,int s,int c)
{
    struct Student stu;
    int fd = open("student.txt", O_RDWR);
    if (fd == -1) 
    {
        perror("Error opening file");
        return;
    }
    off_t offset = s * sizeof(struct Student);
    struct flock lock;
    readLockStudent(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    if(read(fd,&stu,sizeof(struct Student))!=sizeof(struct Student))
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0);
        return ; 
    }
    unlock(&lock,fd);
    close(fd);
    stu.courses[c]=0;
    updateStudentToFile(client_socket,&stu);
}
void changeCourseDetails(int client_socket,int id,int d,char *command_buffer)
{
    int fd = open("courses.txt", O_RDONLY);
    if (fd == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0); 
        return;
    }
    struct Course temp;
    off_t offset = id * sizeof(struct Course);
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
    if(d==1)
    {
        int newSeats=atoi(command_buffer);
        int curSeats=temp.currentStudentsCount;
        if(newSeats<curSeats)
        {
            while(temp.currentStudentsCount>newSeats)
            {
                // printf("%d\n",temp.students[curSeats-1]);
                if(temp.students[temp.currentStudentsIndex]==-1)
                {
                    temp.currentStudentsIndex--;
                    continue;
                }
                removeStudentFromCourse(client_socket,temp.students[temp.currentStudentsIndex],atoi(temp.details[3]));
                temp.students[temp.currentStudentsIndex]=-1;
                temp.currentStudentsIndex--;
                temp.currentStudentsCount--;
            }
        }
    }
    strcpy(temp.details[d],command_buffer);
    updateCourseToFile(client_socket,&temp);
}
int getCourse(int client_socket,struct Faculty *fac)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompt="\nEnter ID of Course you want to change\nExit(enter -1)\n";
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
        if(!isInteger(command_buffer)||fac->courses[atoi(command_buffer)]==0)
        {
            char *msg="Invalid ID\n";
            send(client_socket,msg,strlen(msg),0);
        }
        else
        {
            return atoi(command_buffer);
        }
    }
}
void changeCourseCredits(int client_socket,int id)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompt="Enter new Credits\n";
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
            changeCourseDetails(client_socket,id,2,command_buffer);
            break;
        }
    }
}
void changeMaximumStudents(int client_socket,int id)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompt="Enter max Seats\n";
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
            changeCourseDetails(client_socket,id,1,command_buffer);
            break;
        }
    }
}
void removeCourse(int client_socket,int id,struct Faculty *fac)
{
    int fd = open("courses.txt", O_RDWR);
    if (fd == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0); 
        return;
    }

    off_t offset = id * sizeof(struct Course);
    struct flock lock;
    readLockCourse(&lock,offset,fd);
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0); 
        return;
    }

    struct Course course;
    if (read(fd, &course, sizeof(struct Course)) != sizeof(struct Course)) 
    {
        send(client_socket,"Failed read\n",strlen("Failed read\n"),0); 
        return;  
    }
    if (lseek(fd, offset, SEEK_SET) == -1) 
    {
        send(client_socket,"Failed find\n",strlen("Failed find\n"),0); 
        return;
    }
    unlock(&lock,fd);
    struct Course emptyCourse; 
    memset(&emptyCourse, 0, sizeof(struct Course)); 
    char *inId="-1\0";
    strcpy(emptyCourse.details[3],inId);
    for(int i=0;i<course.currentStudentsIndex;i++)
    {
        if(course.students[i]!=-1)
        {
            removeStudentFromCourse(client_socket,course.students[i],id);
        }
    }
    fac->courses[id]=0;
    updateFacultyToFile(client_socket,fac);
    writeLockCourse(&lock,offset,fd);
    if (write(fd, &emptyCourse, sizeof(struct Course)) != sizeof(struct Course)) 
    {
        send(client_socket,"Failed\n",strlen("Failed\n"),0); 
        return;  
    }
    unlock(&lock,fd);
    close(fd);
}
void changeCourseName(int client_socket,int id)
{
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompt="Enter new Name\n";
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
    changeCourseDetails(client_socket,id,0,command_buffer);
}
int getNextCourseID() 
{
    int fd = open("courses.txt", O_RDONLY);
    if (fd == -1) 
    {
        return 0; 
    }
    off_t offset = lseek(fd, 0, SEEK_END); 
    int numCourses = offset / sizeof(struct Course);
    close(fd);
    return numCourses; 
}
void writeCourseToFile(int client_socket,struct Course* sub) 
{
    int fd = open("courses.txt", O_WRONLY | O_APPEND);
    if (fd == -1) 
    {
        perror("Error opening file");
        return;
    }
    struct flock lock;
    completeLock(&lock,fd);
    if (write(fd, sub, sizeof(struct Course)) != sizeof(struct Course)) 
    {
        perror("Error writing to file");
        return;
    }
    unlock(&lock,fd);
    char *prompt="\nCourse Added Successfully\n";
    send(client_socket,prompt,strlen(prompt),0);
    close(fd);
}
void addCourse(int client_socket,struct Faculty *fac)
{
    struct Course sub;
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char *prompts[3]={"\nEnter Name of the Course : \n","Enter total seats accepted : \n","Enter credits : \n"};
    for(int k=0;k<3;k++) 
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
                sub.details[k][buffer_index] = '\0'; 
                buffer_index = 0;
                if(k>0&&!isInteger(sub.details[k]))
                {
                    send(client_socket,"Invaild...Enter Again\n",strlen("Invaild...Enter Again\n"),0);
                    k--;
                    continue;
                } 
            } 
            else 
            {
                sub.details[k][buffer_index++] = buffer[i];
            }
        }
    }
    char id[5];
    int nextId=getNextCourseID();
    convertIndexToID(nextId,id);
    id[3]='\0';
    id[4]='\0';
    strncpy(sub.details[3],id,4);
    strcpy(sub.faculty,fac->details[0]);
    sub.currentStudentsCount=0;
    sub.currentStudentsIndex=0;
    for(int i=0;i<MAX_STUDENTS;i++)
    {
        sub.students[i]=-1;
    }
    fac->courses[atoi(id)]=1;
    send(client_socket,"Course Id : ",strlen("Course Id : "),0);
    send(client_socket,id,strlen(id),0);
    updateFacultyToFile(client_socket,fac);
    writeCourseToFile(client_socket,&sub);
}
void editCourse(int client_socket,struct Faculty *fac)
{
    int id=getCourse(client_socket,fac);
    char *prompt="\nWhat would you like to do??\nChange Name(Enter 1)\nChange Credits(Enter 2)\nChange maximum students(Enter 3)\nExit(Enter 0)\nRemove Course(Enter 4)\n";
    int modify=1;
    while(modify)
    {
        char role=takeOption(prompt,client_socket);
        switch(role)
        {
            case '1':
                changeCourseName(client_socket,id);
                break;
            case '2':
                changeCourseCredits(client_socket,id);
                break;
            case '3':
                changeMaximumStudents(client_socket,id);
                break;
            case '4':
                removeCourse(client_socket,id,fac);
                break;
            case '0':
                modify=0;
                break;
            default:
                break;
        }
    }
}
void changePasswordFaculty(int client_socket,struct Faculty *fac)
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
            strcpy(fac->details[3],passwords[0]);
            int fd = open("faculty.txt", O_RDWR);
            int index = atoi(fac->details[4]);
            off_t offset = index * sizeof(struct Faculty);
            if (fd == -1||lseek(fd, offset, SEEK_SET) == -1) 
            {
                char *prompt="Failed\n";
                send(client_socket,prompt,strlen(prompt),0);
                break;
            }
            struct Faculty temp;
            char *prompt;
            struct flock lock;
            readLockFaculty(&lock,offset,fd);
            if (read(fd, &temp, sizeof(struct Faculty)) == sizeof(struct Faculty)) 
            {
                unlock(&lock,fd);
                if (strcmp(temp.details[4], fac->details[4]) == 0) 
                {
                    lseek(fd, offset, SEEK_SET);
                    writeLockFaculty(&lock,offset,fd);
                    if(write(fd, fac, sizeof(struct Faculty))==sizeof(struct Faculty))
                    {
                        prompt="Password changed Succefully\n";
                    }
                    else
                    {
                        prompt="Failed\n";
                    }
                    unlock(&lock,fd);
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
void facultyCourses(struct Faculty *fac,int client_socket)
{
    viewCourses(fac->courses,client_socket);   
}
void handleFacultyLoginSuccess(int client_socket,struct Faculty *fac)
{
    send(client_socket,"\nLogin Successfull.....\n",strlen("\nLogin Successfull.....\n"),0);
    char *prompt="\nWhat would you like to do??\nAdd Course(Enter 1)\nEdit Course(Enter 2)\nChange Password(Enter 3)\nView Courses(Enter 4)\nLogOut(Enter 0)\n";
    int login=1;
    while(login)
    {
        char role=takeOption(prompt,client_socket);
        switch(role)
        {
            case '1':
                addCourse(client_socket,fac);
                break;
            case '2':
                editCourse(client_socket,fac);
                break;
            case '3':
                changePasswordFaculty(client_socket,fac);
                break;
            case '4':
                facultyCourses(fac,client_socket);
                break;
            case '0':
                login=0;
                break;
            default:
                break;
        }
    }
}
void handleFacultyLoginFail(int client_socket)
{
    //notify(client_socket,"Login Fail\n");
    char *prompt="Login Fail\n";
    send(client_socket,prompt,strlen(prompt),0);
    return;
}
