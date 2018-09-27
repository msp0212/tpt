#include <stdio.h>
#include <stdint.h>

int main()
{
	char *s = "#include<stdio.h>\n\n"
		"int main()\n"
		"{\n"
		"char *s = \"%s\"\n"
		"printf(s,s)\n"
		"return 0;\n"
		"}\n";
	printf(s,s);
	return 0;
}
