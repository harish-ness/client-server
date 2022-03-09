// C program for the Server Side

#include "client_server.h"

typedef struct client
{
  int new_socketfd;
  struct sockaddr_in client_address;
  char client_path[SIZE];
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

int show_dir_content(char *path, char *buffer)
{
  struct dirent *directory_entry;
  DIR *dirptr = opendir(path);
  if(dirptr == NULL)
  {
    strlcpy(buffer, "Unable to open current directory", SIZE);
    return 1;
  }
  while ((directory_entry = readdir(dirptr)) != NULL)
  {
    struct stat filestat;
    stat(directory_entry->d_name, &filestat);
    strlcat(buffer, get_file_type(directory_entry->d_type), SIZE);
    strlcat(buffer, "\t", SIZE);
    strlcat(buffer, directory_entry->d_name, SIZE);
    strlcat(buffer, "\t", SIZE);
    char temp[1024];
    ctime_r(&filestat.st_mtime, temp);
    strlcat(buffer, temp, SIZE);
  }
  closedir(dirptr);
  return 1;
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
      strlcpy(buffer, ptr->client_path, SIZE);
    }
    else if( strcmp(buffer, "ls") == 0)
    {
      bzero(buffer, SIZE);
      response = show_dir_content(ptr->client_path, buffer);
      if(response == 0)
      {
        break;
      }
    }
    else if( strncmp(buffer, "cd", 2) == 0)
    {
      DIR *dirptr = opendir(buffer+3);
      if(dirptr == NULL)
      {
        bzero(buffer, SIZE);
        strlcpy(buffer, "Directory does not exist.", SIZE);
      }
      else
      {
        closedir(dirptr);
        bzero(ptr->client_path, SIZE);
        strlcpy(ptr->client_path, buffer+3, SIZE);
        bzero(buffer, SIZE);
        strlcpy(buffer, "Directory changed successfully", SIZE);
      }
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
  bzero(buffer, SIZE);
  strlcpy(buffer, "bye", SIZE);
  response = send(ptr->new_socketfd, buffer, SIZE, 0);
  if (response < 0)
    perror("Unable to send message\n");
  free(ptr);
  free(buffer);
  close(ptr->new_socketfd);
  pthread_exit(NULL);
  return NULL;
}

int main()
{
  int socketfd, opt = 1, i = 0;
  struct sockaddr_in address;
  socklen_t address_length;
  pthread_t *mythread[MAX];
  char *buffer;
  
  buffer = (char *)malloc(SIZE);
  if(buffer == NULL)
  {
  	perror("Unable to allocate memory.");
  	exit(EXIT_FAILURE);
  }
  if(getcwd(buffer, SIZE) == NULL)
  {
    perror("Unable to fetch current Directory\n");
    free(buffer);
    exit(EXIT_FAILURE);
  }
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
  if (listen(socketfd, MAX) < 0)
  {
    perror("Unable to listen\n");
    close(socketfd);
    exit(EXIT_FAILURE);
  }
  while(1)
  {
    NODE *ptr = (NODE *)malloc(sizeof(NODE));
    address_length = sizeof(ptr->client_address);
    // Accept first connection request from the queue and create a new connection socket fd
    if((ptr->new_socketfd = accept(socketfd, (struct sockaddr *)&ptr->client_address, (socklen_t*)&address_length))< 0)
    {
      perror("Unable to accept connection request\n");
      close(socketfd);
      exit(EXIT_FAILURE);
    }
    strlcpy(ptr->client_path, buffer, SIZE);
    pthread_t tid[i];
    mythread[i] = &tid[i];
    if( (pthread_create(mythread[i++], NULL, myfunction, (void *)ptr)) != 0)
    {
      perror("Unable to create thread \n");
      break;
    }
  }
  pthread_exit(NULL);
  close(socketfd);
  return 0;
}

