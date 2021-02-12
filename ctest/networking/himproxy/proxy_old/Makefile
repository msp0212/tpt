PROXY_VERSION_MAJOR = 1
PROXY_VERSION_MINOR = 0
PROXY_VERSION_PATCH = 0
PROXY_BUILD_NO = 1
PROXY_BUILD_DATE = $(shell date)

DEPDIR = .deps
DF = $(DEPDIR)/$(*F)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
BIN_NAME = proxy

DEFINES += -DPROXY_VERSION_DETAILS='"MahindraComviva Proxy v$(PROXY_VERSION_MAJOR).$(PROXY_VERSION_MINOR).$(PROXY_VERSION_PATCH) \
			Build\#$(PROXY_BUILD_NO) Date $(PROXY_BUILD_DATE)"' 
INCLUDES := -I $(COMMON_INCL_DIR) -I ./include
LIBRARIES := -L $(COMMON_LIB_DIR) -lsample -lconf_utils -lworkq -lhttp_utils \
		-lrt

CFLAGS += $(DEFINES) $(INCLUDES) $(LIBRARIES)

define MSG
*************************************************
* Proxy built successfully
*************************************************
endef

export MSG

.PHONY : clean

$(BIN_NAME) : $(OBJS) 
	$(CC) -o $@ $(OBJS) $(CFLAGS)
	cp $(BIN_NAME) $(MIG_BIN_DIR) 
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

