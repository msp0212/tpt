ARCH = 32bit
CC = gcc

ifeq ($(ARCH), 32bit)
	CC = gcc -m32
else
	CC = gcc -m64
endif

CFLAGS += -std=c99 -g -Wall

DEPDIR = .deps
DF = $(DEPDIR)/$(*F)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

INCL := -I../include -I../../bson_api/include -I../../bson_api/libbson-1.1.0/out/include/libbson-1.0/
LIBS := -L../ -lmongo_api -L../../bson_api -lbson_api -L../mongo-c-driver-1.1.0/out/lib/ -lmongoc-1.0  -lpthread

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


