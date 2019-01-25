CC = gcc -g -Wall -Werror

DEPDIR = .deps
DF = $(DEPDIR)/$(*F)

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
BIN:= exec_ns

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $^

%.o : %.c                                                                      
	$(CC) -o $@ -c -MMD $< $(INCLUDE)
	@mkdir -p $(DEPDIR)
	@cp $*.d $(DF).P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(DF).P; \
	rm -f $*.d

clean:
	@rm -f $(OBJS) $(BIN)
