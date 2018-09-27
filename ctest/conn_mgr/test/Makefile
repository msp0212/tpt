ARCH = 64bit
CC = /usr/bin/gcc

ifeq ($(ARCH), 32bit)
	CC += -m32
else
	CC += -m64
endif

CFLAGS += -std=c99 -g -Wall

DEPDIR = .deps
DF = $(DEPDIR)/$(*F)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

INCL := -I../include
LIBS := -L../ -lconn_mgr -lpthread

BIN_NAME = test



define MSG
*************************************************
* Test built successfully
*************************************************
endef

export MSG

.PHONY : clean

$(BIN_NAME) : $(OBJS) 
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
	@echo "$$MSG"

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c -MMD $< $(INCL)
	@mkdir -p $(DEPDIR)
	@cp $*.d $(DF).P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(DF).P; \
	rm -f $*.d

-include $(SRCS:%.c=$(DEPDIR)/%.P)

clean : 
	-@rm -f *.o $(BIN_NAME)


