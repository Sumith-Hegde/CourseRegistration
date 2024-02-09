#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<fcntl.h>
#include "components.h"
#include "methods.h"
#include "handleRegister.h"
#include "handleLogin.h"
#include "faculty.h"
#include "student.h"
#include "admin.h"
#define PORT 12345
#define MAX_BUFFER_SIZE 1024
struct user
{
	char userName[51];
	char password[51];
};
void handleAdmin(int client_socket)
{
            int fd=open("admin.txt",O_RDWR);
            char credentials[2][51];
            int size[2];
            char entered[2][51];
            int l=0;
            int success=0;
            char buffer2[1024];
            while(1)
            {
                int count=0;
                while(1)
                {
                    int readCount=read(fd,&buffer2[count],1);
                    count+=readCount;
                    if(count==0||count==1000||buffer2[count-1]=='\n')
                    {
                        break;
                    }
                }
                if(count==0)
                {
                    break;
                }
                for(int i=0;i<count;i++)
                {
                    credentials[l][i]=buffer2[i];
                }
                size[l]=count;
                l++;
                if(l==2)
                {
                    break;
                }
            }
            printf("%s\n",credentials[0]);
            printf("%s\n",credentials[1]);
            printf("%d\n",size[0]);
            printf("%d\n",size[1]);
    		char buffer[1024];
		    char command_buffer[1024];
		    int buffer_index = 0;
		    char *prompts[3]={"Enter UserName : \n","Enter Password : \n"};
		    l=0;
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
                        command_buffer[buffer_index] = '\0';   
                        if(strncmp(command_buffer,credentials[l],size[l]-1)==0)
                        {
                            success++;
                        }
                        l++;
                        buffer_index = 0;
                    } 
                    else 
                    {
                        command_buffer[buffer_index++] = buffer[i];
                    }
                }
		    }
            if(success==2)
            {
                handleAdminLoginSuccess(client_socket);
            }
            else
            {
                handleAdminLoginFail(client_socket);
            }
	
}

void handleFaculty(int client_socket)
{
    char *prompts={"Login(enter 1)\nRegister(enter 2)\nExit(enter 0)"};
    struct Faculty fac;
    int success;
    int show=1;
    while(show)
    {
        char role=takeOption(prompts,client_socket);
        switch(role)
        {
            case '1':
                success=loginFaculty(client_socket,&fac);
                if(success==1)
                {
                    handleFacultyLoginSuccess(client_socket,&fac);
                }
                else
                {
                    handleFacultyLoginFail(client_socket);
                }
                break;
            case '2':
                registerFaculty(client_socket);
                break;
            case '0':
                show=0;
                break;
            default:
                send(client_socket,"Invalid Option\n",strlen("Invalid Option\n"),0);
                break;
        }
    }
}
void handleStudent(int client_socket)
{
    char *prompts={"Login(enter 1)\nRegister(enter 2)\nExit(enter 0)"};
    struct Student stu;
    int success=0;
    int show=1;
    while(show)
    {
        char role=takeOption(prompts,client_socket);
        switch(role)
        {
            case '1':
                success=loginStudent(client_socket,&stu);
                if(success==1)
                {
                    handleStudentLoginSuccess(client_socket,&stu);
                }
                else
                {
                    handleStudentLoginFail(client_socket);
                }
                break;
            case '2':
                registerStudent(client_socket);
                break;
            case '0':
                show=0;
                break;
            default:
                send(client_socket,"Invalid Option\n",strlen("Invalid Option\n"),0);
                break;
        }
    }
}
void handleClient(int client_socket) 
{   
    char buffer[1024];
    char command_buffer[1024];
    int buffer_index = 0;
    char role=' ';
    char *prompts={"Who are you?\nAdmin(enter 1)\nFaculty(enter 2)\nStudent(enter 3)\nEnter 0 to exit\n"};
    while(1)
    {
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
        role=command_buffer[0];
        switch(role)
        {
            case '1':
                handleAdmin(client_socket);
                break;
            case '2':
                handleFaculty(client_socket);
                break;
            case '3':
                handleStudent(client_socket);
                break;
            case '0':
                shutdown(client_socket, SHUT_RDWR);
                break;
            default:
                send(client_socket,"Invalid Option\n",strlen("Invalid Option\n"),0);
                break;
        }
    }
}

int main() 
{
  	int serverSocket;
  	struct sockaddr_in serverAddr;
  	socklen_t clientAddrLen = sizeof(struct sockaddr_in);
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  	if (serverSocket == -1) 
 	{
    		perror("Error creating socket");
    		exit(EXIT_FAILURE);
  	}
  	memset(&serverAddr, 0, sizeof(serverAddr));
  	serverAddr.sin_family = AF_INET;
  	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	serverAddr.sin_port = htons(PORT);
  	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) 
  	{
    		perror("Error binding socket");
    		close(serverSocket);
    		exit(EXIT_FAILURE);
  	}
  	if (listen(serverSocket, 5) == -1) 
  	{
    		perror("Error listening for connections");
    		close(serverSocket);
    		exit(EXIT_FAILURE);
  	}
  	printf("Server is listening on port %d...\n", PORT);
  	while (1) 
  	{
    		int clientSocket;
    		clientSocket = accept(serverSocket, (struct sockaddr *)&serverAddr, &clientAddrLen);
    		if (clientSocket == -1) 
    		{
      			perror("Error accepting connection");
      			continue; 
    		}
    		printf("Client connected.\n");
    		pid_t pid = fork();
    		if (pid == -1) 
    		{
      			perror("Error creating child process");
      			close(clientSocket);
      			continue; 
    		}
    		if (pid == 0) 
    		{
      			close(serverSocket); 
      			handleClient(clientSocket);
      			exit(EXIT_SUCCESS); 
    		} 
  	}
  	close(serverSocket);
  	return 0;
}
