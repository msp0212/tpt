#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 256

/*
 * -remember the index of last occurence of each char that exists in the patter
 * -this table will be used while searching to skip the chars in the text
 * */
void remember_last_occurence(int last_occurence[], char *pat, int pat_len)
{
	int i;

	for (i = 0; i < ALPHABET_SIZE; i++) {
		last_occurence[i] = -1;
	}
	for (i = 0; i < pat_len; i++) {
		last_occurence[(int)pat[i]] = i;
	}
}

/* implementation of boyer-moore algorithm for string matching
 * using bad char heuristics.
 *
 * example1:
 * text:	gcaatgcctatgtgacc
 * patt:	tatgtg
 *
 * matching is done from the _tail_ i.e last character of the pattern is matched
 * with text[pat_len - 1]
 *
 * there are 2 cases when the first mismatch occurs.
 * case1: mismatched char from the text is present in the pattern
 *  -find the last occurence of the mismatched char in the pattern
 *  -skip portion of the text such that mismatched char gets aligned with the
 *   last occurence of it in the pattern
 *  -continue matching again
 * case2: mismatched char from the text is not present in the pattern
 *  -skip portion of the text such that we move past the mismatched char
 *  -continue matching again
 * */
char *boyer_moore_badchar_strstr(char *text, char *pat)
{
	char *result;
	int last_occurence[ALPHABET_SIZE];
	int text_len;
	int pat_len;
	int text_inx;
	int pat_inx;
	int last_inx;

	text_len = strlen(text);
	pat_len = strlen(pat);

	remember_last_occurence(last_occurence, pat, pat_len);
	
	result = NULL;
	text_inx = 0;
	pat_inx = pat_len - 1;
	while (text_inx <= (text_len - pat_len)) {
		if (text[text_inx + pat_inx] == pat[pat_inx]) {
			pat_inx--;
		} else {
			/*mismatch occured at index = text_inx + pat_inx*/
			last_inx = last_occurence[(int)text[text_inx + pat_inx]];
			if (last_inx < 0) {
				/* mismatch char is not present in pattern
				 * move past mismatched index*/
				text_inx = text_inx + pat_inx + 1;
			} else {
				/*mismatched char is present in pattern
				 * align the mismatched index with last
				 * occurence of it in pattern*/
				text_inx = text_inx + pat_inx - last_inx;
			}
			/*contine matching again from text_inx*/
			pat_inx = pat_len - 1;
		}
		if (pat_inx < 0) {
			printf("Pattern found at index %d\n", text_inx);
			result = text + text_inx;
			break;
		}
	}
	return result;
}


int main(int argc, char *argv[])
{
	char *result = NULL;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <input string> <search string>\n",
					argv[0]);
		exit(1);
	}

	printf("text:\t%s\n", argv[1]);
	printf("patt:\t%s\n", argv[2]);
	
	result = boyer_moore_badchar_strstr(argv[1], argv[2]);
	printf("result:\t%s\n", result ? result : "Not found");

	return 0;
}
