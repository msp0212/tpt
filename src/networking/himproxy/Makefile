CC = gcc -g -Wall
ARCH = 64bit

ifeq ($(ARCH), 32bit) 
	CC += -m64	
else 
	CC += -m32
endif

HIMPROXY_VERSION_MAJOR = 1
HIMPROXY_VERSION_MINOR = 0
HIMPROXY_VERSION_PATCH = 0
HIMPROXY_BUILD_NO = 1
HIMPROXY_BUILD_DATE = $(shell date)

DEPDIR = .deps
DF = $(DEPDIR)/$(*F)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
BIN_NAME = himproxy

DEFINES += -DHIMPROXY_VERSION_DETAILS='"HimProxy v$(HIMPROXY_VERSION_MAJOR)."\
	   	"$(HIMPROXY_VERSION_MINOR).$(HIMPROXY_VERSION_PATCH)"\
		" Build\#$(HIMPROXY_BUILD_NO) Date [$(HIMPROXY_BUILD_DATE)]"' 
INCLUDES := -I ./include
LIBRARIES := 

CFLAGS += $(DEFINES) $(INCLUDES) $(LIBRARIES)

define MSG
*************************************************
* HimProxy built successfully
*************************************************
endef

export MSG

.PHONY : clean

$(BIN_NAME) : $(OBJS) 
	$(CC) -o $@ $(OBJS) $(CFLAGS)
	@echo "$$MSG"

%.o : %.c
	$(CC) -o $@ -c -MMD $< $(CFLAGS)
	@mkdir -p $(DEPDIR)
	@cp $*.d $(DF).P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(DF).P; \
	rm -f $*.d

-include $(SRCS:%.c=$(DEPDIR)/%.P)

clean : 
	-@rm -f *.o $(BIN_NAME)

