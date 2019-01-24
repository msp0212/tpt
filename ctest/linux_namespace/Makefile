all: exec_ns

.phony=clean

exec_ns: namespace.c
	gcc -Wall $^ -o $@

clean:
	@rm -f exec_ns
