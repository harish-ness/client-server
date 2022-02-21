# Makefile

# variable declarations
# For C compiler
CC = gcc

# C flags
CFLAGS = -Wall -Wextra -Werror -g

# Compile
client: tcp_client.c
	$(CC) $(CFLAGS) tcp_client.c -o client -lbsd

server: tcp_server.c
	$(CC) $(CFLAGS) tcp_server.c -o server -lbsd

# Remove unwanted files
clean:
	rm *.o
