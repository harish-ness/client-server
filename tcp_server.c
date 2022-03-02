// C program for the Server Side

#include "client_server.h"

typedef struct client
{
  int new_socketfd;
  struct sockaddr_in client_address;
  char *client_path;
}NODE;

char *get_file_type(unsigned char n)
{
  if(n == DT_DIR)
    return "Directory";
  else if(n == DT_BLK)
    return "Block Device";
  else if(n == DT_CHR)
    return "Character Device";
  else if(n == DT_FIFO)
    return "Named pipe FIFO";
  else if(n == DT_LNK)
    return "Symbolic Link";
  else if(n == DT_REG)
    return "Regular File";
  else if(n == DT_UNKNOWN)
   return "Unknown";
  return 0;
}

void *myfunction(void *p_client_socket)
{
  NODE *ptr = (NODE *)p_client_socket;
  char *buffer;
  int response;

  buffer = (char *)malloc(SIZE);
  if(buffer == NULL)
  {
    perror("Unable to allocate a memory.\n");
    close(ptr->new_socketfd);
    exit(0);
  }
  while(1)
  {
    response = read(ptr->new_socketfd, buffer, SIZE);
    if (response < 0)
    {
      perror("Unable to receive message\n");
      break;
    }
    if( strcmp(buffer, "bye") == 0)
    {
      break;
    }
    else if( strcmp(buffer, "pwd") == 0)
    {
      bzero(buffer, SIZE);
      strlcpy(buffer, (char *)ptr->client_path, SIZE);
    }
    else if( strcmp(buffer, "ls") == 0)
    {
      struct stat filestat;
      struct dirent *director_entry;
      bzero(buffer, SIZE);
      DIR *dirptr = opendir(".");
      if (dirptr == NULL)
      {
        strlcpy(buffer, "Unable to open current directory", SIZE);
      }
      else
      for(int i=0; (director_entry = readdir(dirptr)) != NULL; i++)
      {
        if(stat(director_entry->d_name, &filestat) != 0)
        {
          strlcpy(buffer, "Unable to open current directory", SIZE);
          break;
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
      bzero(buffer, SIZE);
      bzero(ptr->client_path, SIZE);
      strlcpy(ptr->client_path, buffer+3, SIZE);
      strlcpy(buffer, "Directory changed successfully", SIZE);
    }
    else
    {
      bzero(buffer, SIZE);
      strlcpy(buffer, "Invalid request, request must be (pwd, ls, cd, bye)", SIZE);
    }
    response = send(ptr->new_socketfd, buffer, SIZE, 0);
    if (response < 0)
    {
      perror("Unable to send message\n");
      break;
    }
    bzero(buffer, SIZE);
  }
  free(buffer);
  close(ptr->new_socketfd);
  pthread_exit(NULL);
  return NULL;
}

int main()
{
  int socketfd, opt = 1, i = 0, temp;
  struct sockaddr_in address;
  socklen_t address_length;
  pthread_t *mythread[MAX];
  char *name, *ser_name, *hostname;
  
  name = (char *)malloc(SIZE);
  if(name == NULL)
  {
    perror("Unable to allocate a memory.\n");
    exit(0);
  }
  ser_name = (char *)malloc(SIZE);
  if(ser_name == NULL)
  {
    perror("Unable to allocate a memory.\n");
    exit(0);
  }
  hostname = (char *)malloc(SIZE);
  if(hostname == NULL)
  {
    perror("Unable to allocate a memory.\n");
    exit(0);
  }
  strlcat(ser_name, "/home/", SIZE);
  if( (temp = getlogin_r(name, SIZE)) != 0 )
  {
  	perror("Unable to fetch username \n");
  	exit(EXIT_FAILURE);
  }
  strlcat(ser_name, name, SIZE);
  strlcat(ser_name, "/", SIZE);
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
  // Bind the socket to the port 54321 and address
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
  while(1)
  {
    bzero(name, SIZE);
    NODE *ptr = (NODE *)malloc(sizeof(NODE));
    address_length = sizeof(ptr->client_address);
    // Accept first connection request from the queue and create a new connection socket fd
    if((ptr->new_socketfd = accept(socketfd, (struct sockaddr *)&ptr->client_address, (socklen_t*)&address_length))< 0)
    {
      perror("Unable to accept connection request\n");
      close(socketfd);
      exit(EXIT_FAILURE);
    }
    if(getnameinfo((struct sockaddr *)&ptr->client_address, sizeof(ptr->client_address), hostname, SIZE, NULL,0,0) < 0)
    {
      perror("Unable to get hostname");
      strlcpy(name, ser_name, SIZE);
      strlcat(name, "localhost", SIZE);
    }
    else
    {
      strlcpy(name, ser_name, SIZE);
      strlcat(name, hostname, SIZE);
    }
    ptr->client_path = (char *)malloc(SIZE);
    strlcpy(ptr->client_path, name, SIZE);
    pthread_t tid[i];
    mythread[i] = &tid[i];
    if( (pthread_create(mythread[i++], NULL, myfunction, (void *)ptr)) != 0)
    {
      perror("Unable to create thread \n");
      break;
    }
    bzero(hostname, SIZE);
  }
  pthread_exit(NULL);
  close(socketfd);
  return 0;
}

