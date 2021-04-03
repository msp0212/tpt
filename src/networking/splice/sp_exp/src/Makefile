#Simple Proxy 
#Mohit Singh 06 Feb 2014

MESSAGE="SimpleProxy built successfully"

#CC  = gcc -m32 -g3 -Wall -save-temps
#CC  = gcc -m32 -g3 -Wall 
CC  = gcc -g3 -Wall 

BINARY_NAME = sp_proxy
INCL = -I ./include
LIBS  = -lpthread -lrt

# It's a standard for every makefile to have variable which is a list of all object file names 
#OBJS = sproxy_main.o sproxy_globals.o sproxy_core.o sproxy_http_handler.o sproxy_net.o sproxy_http_parser.o sproxy_dns_lookup.o  sproxy_log.o
OBJS = sproxy_main.o sproxy_globals.o sproxy_core.o sproxy_net.o sproxy_log.o sproxy_splice.o
all : sproxy

sproxy : $(OBJS)
	$(CC) -o $(BINARY_NAME) $(OBJS) $(INCL) $(LIBS) 
#	$(CC) -o $(BINARY_NAME) $(OBJS) $(INCL) $(LIBS)
	@echo "*******************************************************************"
	@echo $(MESSAGE)
	@echo "*******************************************************************"
sproxy_main.o : sproxy_main.c 
	$(CC) -c sproxy_main.c $(INCL) $(LIBS)

sproxy_globals.o : sproxy_globals.c 
	$(CC) -c sproxy_globals.c $(INCL) $(LIBS)

sproxy_log.o : sproxy_log.c 
	$(CC) -c sproxy_log.c $(INCL) $(LIBS)

sproxy_core.o : sproxy_core.c 
	$(CC) -c sproxy_core.c $(INCL) $(LIBS)

sproxy_net.o : sproxy_net.c 
	$(CC) -c sproxy_net.c $(INCL) $(LIBS)

sproxy_splice.o : sproxy_splice.c 
	$(CC) -c sproxy_splice.c $(INCL) $(LIBS)
.PHONY : clean
clean:
	-@rm *.o $(BINARY_NAME)
