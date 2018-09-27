#tcmalloc test
#Mohit Singh 20 Aug 2013

MESSAGE="tc malloc test utilities built successfully!!!"

CC  = gcc -g3
LIB_PATH = /root/data/tpt/libs/
CFLAGS  += -I./include -lpthread

all :
	$(CC) -o old malloc_test.c $(CFLAGS) $(LIB_PATH)workq.a
	#$(CC) -o new tcmalloc_test.c $(CFLAGS) $(LIB_PATH)workq.a -ltcmalloc 
	$(CC) -o new tcmalloc_test.c $(CFLAGS) $(LIB_PATH)workq.a -ltcmalloc -lnuma
	@echo "*******************************************************************"
	@echo $(MESSAGE)
	@echo "*******************************************************************"
clean:
	-@rm old new

