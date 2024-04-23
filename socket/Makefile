all: aesdsocket

CROSS_COMPILE=
CC ?= $(CROSS_COMPILE)gcc

FLAGS ?= -Wall -Werror -g

aesdsocket:
	$(CC) $(FLAGS) -o socketserver server.c

clean: 
	rm -f *.o socketserver
