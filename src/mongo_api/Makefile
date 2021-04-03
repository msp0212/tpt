ARCH = 32bit
CC = gcc 

ifeq ($(ARCH), 32bit) 
	CC = gcc -m32
else 
	CC = gcc -m64
endif

CFLAGS += -std=c99 -g -Wall

LIB_NAME := libmongo_api
VERSION_MAJOR := 1
VERSION_MINOR := 0

DEPDIR = .deps
DF = $(DEPDIR)/$(*F)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

INCL := -I./include -I./mongo-c-driver-1.1.0/out/include/libmongoc-1.0 -I./mongo-c-driver-1.1.0/out/include/libbson-1.0 
LIBS := -L./mongo-c-driver-1.1.0/out/lib -lmongoc-1.0 

SONAME := $(LIB_NAME).so.$(VERSION_MAJOR)
REALNAME := $(LIB_NAME).so.$(VERSION_MAJOR).$(VERSION_MINOR)
LIB_HEADER := ./include/mongo_api.h


define MSG
*************************************************
* MONGO API library built successfully
*************************************************
endef

export MSG

.PHONY : all clean

all : $(REALNAME) $(LIB_NAME).a
	@echo "$$MSG"

$(LIB_NAME).a : $(OBJS) 
	ar -rcvs $(LIB_NAME).a $(OBJS)

$(REALNAME) : $(OBJS)
	$(CC) $(CFLAGS) --shared -Wl,-soname,$(SONAME) -o $(REALNAME) $(OBJS) $(LIBS)
	cp $(REALNAME) $(COMMON_LIB_DIR) 
	ln -sf $(COMMON_LIB_DIR)/$(REALNAME) $(COMMON_LIB_DIR)/$(LIB_NAME).so
	mkdir -p $(COMMON_INCL_DIR)/mongo_api
	cp  $(LIB_HEADER) $(COMMON_INCL_DIR)/mongo_api

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c -fPIC -MMD $< $(INCL)
	@mkdir -p $(DEPDIR)
	@cp $*.d $(DF).P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(DF).P; \
		rm -f $*.d

-include $(SRCS:%.c=$(DEPDIR)/%.P)

clean :
	@rm -f *.o $(LIB_NAME)*
