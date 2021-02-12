CC = gcc

DYN_LIB_NAME = libtest.so
MAJ_VER = 1
MIN_VER = 0

SONAME = $(DYN_LIB_NAME).$(MAJ_VER)
REALNAME = $(DYN_LIB_NAME).$(MAJ_VER).$(MIN_VER)

OBJS = my_lib.o

all : $(OBJS)
	$(CC) --shared -Wl,-soname,$(SONAME) -o $(REALNAME) $(OBJS)
	ln -sf $(REALNAME) $(SONAME)
	ln -sf $(SONAME) $(DYN_LIB_NAME)
my_lib.o :
	$(CC) -g -c -Wall -fPIC my_lib.c

clean :
	-@rm *.o *.so*
