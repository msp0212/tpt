#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dlfcn.h>

int main (int argc, char **argv)
{
	void *so_handle;
	int (*fn_ptr)(char *);
	char *so_filename = NULL;
	char so_symbol[32] = {0, };
	char *error = NULL;

	if (argc != 2) {
		fprintf(stderr, "Invalid arguments to the program !!!\nUsage : %s <so_filename>\n", argv[0]);
		exit(1);
	}
	so_filename = strdup(argv[1]); //TODO free this string
	while (1) {
		so_handle = dlopen(so_filename, RTLD_LAZY);
		if (!so_handle) {
			fprintf(stderr, "Error [%s] in dlopen for file [%s]\n", dlerror(), so_filename);
			exit(1);
		}

		strncpy(so_symbol, "do_something", sizeof(so_symbol));
		fn_ptr = dlsym(so_handle, so_symbol);
		if ((error = dlerror()) != NULL) {
			fprintf(stderr, "Error [%s] in loading symbol [%s]\n", dlerror(), so_symbol);
			exit(1);
		}

		(*fn_ptr)("##### New Way to Say Hurray #####");

		dlclose(so_handle);
		sleep(5);
	}
	free(so_filename);
	return 0;
}
