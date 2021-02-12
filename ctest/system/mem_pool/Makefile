ASAN ?= 0
DEBUG ?= 0

CC = gcc

DEFINES =
ifeq ($(DEBUG), 1)
	DEFINES += -DARGO_TEST_DEBUG
endif
INCLUDES := -I .

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
BIN = argo_test_mem_pool

CFLAGS = -Wall -Werror -g
ifeq ($(ASAN), 1)
	CFLAGS += -fsanitize=address
endif

.PHONY: clean

%.o : %.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c -o $@ $?

all: $(BIN)

$(BIN) : $(OBJS)
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $^

clean:
	-@rm -f $(BIN) *.o
