/*
 * Given a String str , print all the combination of the string with considering upper as well as lower combinations of all the characters.
 * Explanation:- lets say String str = "the"  combination are like :- The , tHe , thE etc.
 * */

#include <stdio.h>
#include <string.h>
#include <ctype.h>


void print_combination(char *str, int beg, int end)
{
	if (beg == end) {
		printf("%s\n", str);
		return;
	}
	str[beg] = toupper(str[beg]);
	print_combination(str, beg + 1, end);
	str[beg] = tolower(str[beg]);
	print_combination(str, beg + 1, end);
}

int main(int argc, char *argv[])
{
	char *input = argv[1];
	print_combination(input, 0, strlen(input));
	return 0;

}
