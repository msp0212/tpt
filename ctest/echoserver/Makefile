MESSAGE="Echo Program Built Successfully"
MESSAGE1 = " ------------------------- "

CC = gcc -m32

CFLAGS = -g -I./include -lpthread

all: exe 
	@echo $(MESSAGE1)
	@echo $(MESSAGE)
	@echo $(MESSAGE1)

exe: server client server2 client2

server: server.o myServer.o
	${CC} $(CFLAGS) -o $@ server.o 

client: client.o myClient.o
	${CC} $(CFLAGS) -o $@ client.o 

server2: myServer.o
	${CC} $(CFLAGS) -o $@ myServer.o 

client2: myClient.o
	${CC} $(CFLAGS) -o $@ myClient.o 

clean:
	-@rm *.o
	-@rm server client server2 client2
