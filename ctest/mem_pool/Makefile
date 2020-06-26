ASAN ?= 0
DEBUG ?= 0

CC = gcc
DEFINES =
BIN = amp
SRC = argo_mem_pool.c

CFLAGS = -Wall -Werror -g
ifeq ($(ASAN), 1)
	CFLAGS += -fsanitize=address
endif

ifeq ($(DEBUG), 1)
	DEFINES += -DDEBUG
endif

.PHONY: clean

all: $(BIN)

$(BIN) : $(SRC)
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $^

clean:
	rm -f $(BIN)
