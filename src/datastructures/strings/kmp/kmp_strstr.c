#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
 * example1:
 * patt:	onions
 *
 * computing the lps array: 
 * -find the length of longest prefix which also a suffix for patt[0 to i]
 *  where 0 <= i < length(patt)
 * -Only proper prefix and suffix should be considered i.e. 
 *  excluding the ones which contains all chars from the pattern
 *
 * patt[0 to 0]:o
 * 	pre: 
 * 	suff:
 * 	lps[0] = 0
 * patt[0 to 1]: on
 * 	pre: o
 * 	suff: n
 *	lps[1] = 0
 * patt[0 to 2]: oni
 * 	pre: o, on
 * 	suff: i, ni
 * 	lps[2] = 0
 * patt[0 to 3]: onio
 * 	pre: o, on, oni
 * 	suff: o, io, nio
 * 	lps[3] = 1
 * patt[0 to 4]: onion
 * 	pre: o, on, onio
 * 	suff: n, on, ion, nion
 * 	lps[4] = 2
 * patt[0 to 5]: onions
 * 	pre: o, on, onio, onion
 * 	suff: s, ns, ons, ions, nions
 * 	lps[5] = 0
 * lps_arr[] =	000120
 *
 *
 * example2:
 * patt:	aabaaac
 * lps_arr[]:	0101220
 *
 * */

int *create_lps_array(char *patt, int patt_len)
{
	int *lps;
	int pre;
	int suff;

	lps = malloc(sizeof(*lps) * patt_len);
	assert(lps != NULL);
	
	pre = 0; /*prefix length*/ 
	suff = 1; /*suffix index*/
	/* lps at index 0 is always 0
	 * because there are no proper prefixes for this case*/
	lps[0] = 0;
	while (suff < patt_len) {
		if (patt[pre] == patt[suff]) {
			/* match:
			 * -increment prefix length
			 * -set lps at index suff to current prefix length
			 * -increment suffix index */
			pre++;
			lps[suff] = pre;
			suff++;
		} else {
			/* mismatch:
			 * -if prefix length is non zero
			 *  set prefix length to lps[pre - 1]
			 *  because lps[pre - 1] number of previous chars 
			 *  would have already matched before this step.
			 *  see example2 above
			 * -else
			 *  set lps at index suff to zero since there was a 
			 *  mismatch and no previous characters have matched
			 *  */
			if (pre) {
				pre = lps[pre - 1];
			} else {
				lps[suff] = 0;
				suff++;
			}
		}
	}
	return lps;
}

void print_lps_array(int *lps, int len)
{
	int i;

	printf("lps:\t");
	for(i = 0; i < len; i++) {
		printf("%d", lps[i]);
	}
	printf("\n");
}

/*
 * example1:
 * text:	onionionsps
 * patt:	onions
 * 
 * lps[] = 	000120
 *
 *
 * */

char *kmp_strstr(char *text, char *patt)
{
	int *lps;
	char *result;
	int text_len;
	int patt_len;
	int text_inx;
	int patt_inx;
	
	text_len = strlen(text);
	patt_len = strlen(patt);

	lps = create_lps_array(patt, patt_len);
	print_lps_array(lps, patt_len);
	
	result = NULL;
	text_inx = 0;
	patt_inx = 0;

	while (text_inx < text_len) {
		if (text[text_inx] == patt[patt_inx]) {
			/* match:
			 * increment both indexes and keep matching till we
			 * find a mismatch or all chars in patt are matched
			 * */
			text_inx++;
			patt_inx++;
		} else {
			/* mismatch
			 * -if patt_inx is non zero i.e. 
			 *  some chars in patt were already matched which means
			 *  pattern could be found before text_inx also.
			 *  set patt_inx to value just after the portion 
			 *  in patt that has been matched already.
			 *  lps array is looked up for this. lps[patt_inx -1]
			 *  contains longest prefix before this index that is
			 *  also a suffix. so skip matching lps[patt_inx - 1]
			 *  number of characters
			 * -else
			 *  pattern cannot be found before text_inx.
			 *  start the seach again from next index onwards.
			 *  */
			if (patt_inx) {
				patt_inx = lps[patt_inx - 1];
			} else {
				text_inx++;
			}
		}
		if (patt_inx == patt_len) {
			printf("Found patt at index %d\n", text_inx - patt_len);
			result = text + text_inx - patt_len;
			break;
		}
	}

	return result;
}

int main(int argc, char *argv[])
{
	char *result;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <input string> <search string>\n",
					argv[0]);
		exit(1);
	}

	printf("text:\t%s\n", argv[1]);
	printf("patt:\t%s\n", argv[2]);
	
	result = kmp_strstr(argv[1], argv[2]);
	printf("result:\t%s\n", result);

	return 0;
}
