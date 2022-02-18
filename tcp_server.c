// C program for the Server Side

#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<bsd/string.h>
#define PORT 54321
#define SIZE 1024
#define MAX 128

char *get_file_type(int n)
{
	if(n == 4)
		return "Directory";
	else
		return "Regular File";
}

void *myfunction(void *p_client_socket)
{
	char *buffer;
	int response, new_socketfd;

	new_socketfd = *((int *)p_client_socket);
	free(p_client_socket);
	buffer = (char *)malloc(SIZE);
	while(1)
	{
		response = read(new_socketfd, buffer, SIZE);
		if (response < 0)
		{
			perror("Unable to receive message\n");
			free(buffer);
			close(new_socketfd);
			break;
		}
		if( strcmp(buffer, "bye") == 0)
		{

			free(buffer);
			close(new_socketfd);
			return NULL;
		}
		else if( strcmp(buffer, "pwd") == 0)
		{
			bzero(buffer, SIZE);
			if(getcwd(buffer, SIZE) == NULL)
			{
				perror("Unable to fetch current Directory\n");
				free(buffer);
				close(new_socketfd);
				return NULL;
			}
		}
		else if( strcmp(buffer, "ls") == 0)
		{
			struct stat filestat;
			struct timespec st_birthtimespec;
			struct dirent *director_entry;

			bzero(buffer, SIZE);
			DIR *dirptr = opendir(".");
			if (dirptr == NULL)
			{
				perror("Unable to open current directory \n");
				free(buffer);
				close(new_socketfd);
				return NULL;
			}
			for(int i=0; (director_entry = readdir(dirptr)) != NULL; i++)
			{
				if(stat(director_entry->d_name, &filestat) != 0)
				{
					perror("Unable to fetch records\n");
					free(buffer);
					close(new_socketfd);
					return NULL;
				}
				strlcat(buffer, get_file_type(director_entry->d_type), SIZE);
				strlcat(buffer, "\t", SIZE);
				strlcat(buffer, director_entry->d_name, SIZE);
				strlcat(buffer, "\t", SIZE);
				strlcat(buffer, ctime(&filestat.st_ctime), SIZE);
				strlcat(buffer, "\n", SIZE);
			}
			closedir(dirptr);
		}	
		else if( strncmp(buffer, "cd", 2) == 0)
		{
			char *token = strtok(buffer, " ");
			if( chdir(strtok(NULL, "\0")) == 0)
			{
				bzero(buffer, SIZE);
				strlcpy(buffer, "Directory changed successfully", SIZE);
			}
			else
			{
				bzero(buffer, SIZE);
				strlcpy(buffer, "Unable to changed the Directory", SIZE);
			}
		}
		else
		{
			bzero(buffer, SIZE);
			strlcpy(buffer, "Invalid request, request must be (pwd, ls, cd, bye)", SIZE);
		}
		response = send(new_socketfd, buffer, SIZE, 0);
		if (response < 0)
		{
			perror("Unable to send message\n");
			free(buffer);
			close(new_socketfd);
			return NULL;
		}
		bzero(buffer, SIZE);
	}
	free(buffer);
	close(new_socketfd);
	return NULL;
}

int main()
{
	int socketfd, new_socketfd, opt = 1, i = 0;
	struct sockaddr_in address, new_address;
	socklen_t address_length;
	size_t response;
	pthread_t *mythread[MAX];

	// Creating socket file descriptor	socketfd = socket(domain, type, IP)
	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("Socket creation failed \n");
		exit(EXIT_FAILURE);
	}
	// Forcefully attaching socket to the port 54321 or Reused Port and address (optional)
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0 )
	{
		perror("Unable to set socket \n");
		close(socketfd);
		exit(EXIT_FAILURE);
	}
	memset(&address, '\0', sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	address.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to the port 8080 and address
	if (bind(socketfd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("Unable to bind socket\n");
		close(socketfd);
		exit(EXIT_FAILURE);
	}
	// Wait for the client for connection
	if (listen(socketfd, 10) < 0)
	{
		perror("Unable to listen\n");
		close(socketfd);
		exit(EXIT_FAILURE);
	}
	address_length = sizeof(address);
	while(1)
	{
		// Accept first connection request from the queue and create a new connection socket fd
		if((new_socketfd = accept(socketfd, (struct sockaddr *)&address, (socklen_t*)&address_length)) < 0)
		{
			perror("Unable to accept connection request\n");
			close(socketfd);
			exit(EXIT_FAILURE);
		}	
		int *pclient = malloc(sizeof(int));
		*pclient = new_socketfd;
		pthread_t tid[i];
		mythread[i] = &tid[i];
		if(pthread_create(mythread[i++], NULL, myfunction, pclient) != 0)
		{
			perror("Unable to create thread \n");
		}
	}
	return 0;
}

