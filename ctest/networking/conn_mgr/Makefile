ARCH = 64bit
CC = /usr/bin/gcc 

ifeq ($(ARCH), 32bit) 
	CC +=  -m32
else 
	CC +=  -m64
endif

CFLAGS += -std=c99 -g -Wall

LIB_NAME := libconn_mgr
LIB_DOC_NAME := conn_mgr_doc
VERSION_MAJOR := 1
VERSION_MINOR := 0

DEPDIR = .deps
DF = $(DEPDIR)/$(*F)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

INCL := -I./include
LIBS :=  

SONAME := $(LIB_NAME).so.$(VERSION_MAJOR)
REALNAME := $(LIB_NAME).so.$(VERSION_MAJOR).$(VERSION_MINOR)
LIB_HEADER := ./include/conn_mgr.h


define MSG
*************************************************
* CONN MGR library built successfully
*
* You can use make doc to generate doxygen 
* documentation for this library
*************************************************
endef

define DOC_MSG
************************************************
* DOCUMENTATION generated.
*
* PDF -- conn_mgr_doc.pdf is in ./doc dir
* HTML -- refer html dir in ./doc dir
*************************************************
endef

export MSG DOC_MSG

.PHONY : all lib clean doc


lib : $(REALNAME) $(LIB_NAME).a
	@echo "$$MSG"

all : lib doc

$(LIB_NAME).a : $(OBJS) 
#	ar -rcvs $(LIB_NAME).a $(OBJS)

$(REALNAME) : $(OBJS)
	$(CC) $(CFLAGS) --shared -Wl,-soname,$(SONAME) -o $(REALNAME) $(OBJS) $(LIBS)
	#cp $(REALNAME) $(COMMON_LIB_DIR) 
	#ln -sf $(COMMON_LIB_DIR)/$(REALNAME) $(COMMON_LIB_DIR)/$(LIB_NAME).so
	#cp  $(LIB_HEADER) $(COMMON_INCL_DIR)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c -fPIC -MMD $< $(INCL)
	@mkdir -p $(DEPDIR)
	@cp $*.d $(DF).P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(DF).P; \
		rm -f $*.d
-include $(SRCS:%.c=$(DEPDIR)/%.P)
doc : 
	@doxygen ./doc/do2.conf
	@make -C ./doc/latex > /dev/null 
	@cp ./doc/latex/refman.pdf ./doc/$(LIB_DOC_NAME).pdf
	@echo "$$DOC_MSG"
clean :
	@rm -f *.o $(REALNAME) $(LIB_NAME).a
	-@make -C ./doc/latex clean
	-@rm -rf ./doc/html ./doc/latex
