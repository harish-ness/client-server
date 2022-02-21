// C program for the Server Side
#include "client_server.h"

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

int main()
{
  int socketfd, new_socketfd, opt = 1, response;
  struct sockaddr_in address;
  socklen_t address_length;
  pid_t childpid;
  char *buffer;

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
  address_length = sizeof(address);
  buffer = (char *)malloc(SIZE);
  if(buffer == NULL)
  {
    perror("Unable to allocate a memory.\n");
    close(socketfd);
    exit(0);
  }
  while(1)
  {
    // Accept new connection request from the queue and create a new connection socket fd
    if((new_socketfd = accept(socketfd, (struct sockaddr *)&address, (socklen_t*)&address_length)) < 0)
    {
      perror("Unable to accept connection request\n");
      close(socketfd);
      exit(EXIT_FAILURE);
    }
    if((childpid = fork()) == 0)
    {
      while(1)
      {
        response = read(new_socketfd, buffer, SIZE);
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
          if(getcwd(buffer, SIZE) == NULL)
          {
            perror("Unable to fetch current Directory\n");
            free(buffer);
            close(new_socketfd);
            exit(EXIT_FAILURE);
          }
        }
        else if( strcmp(buffer, "ls") == 0)
        {
          struct stat filestat;
          struct dirent *director_entry;
          bzero(buffer, SIZE);
          DIR *dirptr = opendir(".");
          if (dirptr == NULL)
          {
            perror("Unable to open current directory \n");
            free(buffer);
            close(new_socketfd);
            exit(EXIT_FAILURE);
          }
          for(int i=0; (director_entry = readdir(dirptr)) != NULL; i++)
          {
            if(stat(director_entry->d_name, &filestat) != 0)
            {
              perror("Unable to fetch records\n");
              free(buffer);
              close(new_socketfd);
              exit(EXIT_FAILURE);
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
          if( chdir(buffer+3) == 0)
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
          break;
        }
        bzero(buffer, SIZE);
      }
    }
  }
  free(buffer);
  close(socketfd);
  close(new_socketfd);
  return 0;
}

