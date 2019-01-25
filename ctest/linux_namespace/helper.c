#include <stdio.h>
#include <stdlib.h>

void exec_cmd(char *cmd)
{
	printf("%s\n", cmd);
	system(cmd);
}
