MESSAGE="Echo Program Built Successfully"
MESSAGE1 = " ------------------------- "

CC = gcc

CFLAGS = -g -I./include -lpthread

all: exe 
	@echo $(MESSAGE1)
	@echo $(MESSAGE)
	@echo $(MESSAGE1)

exe: server2 client2

server2: myServer.o
	${CC} $(CFLAGS) -o $@ myServer.o 

client2: myClient.o
	${CC} $(CFLAGS) -o $@ myClient.o 

clean:
	-@rm *.o
	-@rm server2 client2
