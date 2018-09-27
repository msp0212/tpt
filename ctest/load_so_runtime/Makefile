all :
	gcc -g -Wall -o test test.c -L ./lib -ltest
	gcc -g -Wall -o ld_lib load_lib.c -ldl
clean :
	-@rm test
	-@rm ld_lib
