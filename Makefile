# Makefile

# variable declarations
# For C compiler
CC = gcc

# C flags
CFLAGS = -Wall -Wextra -Werror -g

# Compile
client_server: tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c -o server -lbsd -lpthread
	$(CC) $(CFLAGS) tcp_client.c -o client -lbsd

# Remove unwanted files
clean:
	rm *.o
