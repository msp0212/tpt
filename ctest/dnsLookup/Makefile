#DNS lookup
#Mohit Singh 06 Feb 2014

MESSAGE="dns_lookup built successfully"

CC  = gcc -m32 -g3 -Wall
#CFLAGS  += -lpthread
all :
	$(CC) dns_lookup.c -o resolv $(CFLAGS) 
	@echo "*******************************************************************"
	@echo $(MESSAGE)
	@echo "*******************************************************************"
clean:
	-@rm  resolv
