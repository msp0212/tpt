CC = gcc -g -Wall 

all: read_write async_read_write

read_write: read_write.c 
	$(CC) -o read_write read_write.c
async_read_write: async_read_write.c 
	$(CC) -o async_read_write async_read_write.c -lpthread -lrt
clean:
	-@rm -f read_write async_read_write
