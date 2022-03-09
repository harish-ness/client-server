// C program for the Client Side

#include "client_server.h"

int main()
{
  char *buffer;
  int socketfd, response;
  struct sockaddr_in address;

  // Creating socket file descriptor	socketfd = socket(domain, type, IP)
  if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Socket creation failed \n");
    return -1;
  }
  memset(&address, '\0', sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;
  
  // Connection request to connect to the server
  if (connect(socketfd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    perror("Unable to Connect Server \n");
    close(socketfd);
    return -1;
  }
  buffer = (char *)malloc(SIZE);
  if(buffer == NULL)
  {
    perror("Unable to allocate a memory.\n");
    close(socketfd);
    exit(0);
  }
  while(1)
  {
    printf("\n Enter your choice: ");		// cd /home/harish	1000
    scanf("%s", &buffer[0]);
    if( strcmp(buffer, "cd") == 0)
    {
      char *temp = (char *)malloc(SIZE);
      if(temp == NULL)
      {
        perror("Unable to allocate a memory.\n");
        break;
      }
      printf(" Enter Directory path: ");
      scanf("%s", &temp[0]);
      strlcat(buffer, " ", SIZE);
      strlcat(buffer, temp, SIZE);
      free(temp);
    }
    response = send(socketfd, buffer, SIZE, 0 );
    if (response < 0)
    {
      perror("Unable to send message\n");
      break;
    }
    if( strcmp(buffer, "bye") == 0)
    {
      break;
    }
    response = read(socketfd, buffer, SIZE);
    if (response < 0)
    {
      perror("Unable to receive message\n");
      break;
    }
    if( strcmp(buffer, "bye") == 0)
    {
      break;
    }
    printf("\n From server: %s \n", buffer);
    bzero(buffer, SIZE);
  }
  free(buffer);
  close(socketfd);
  return 0;
}

