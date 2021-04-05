#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* problem:
 * given a string, rearrange the chars in such a way that no two adjacent chars
 * are same
 *
 * example1
 * input:	aaabc
 * output:	abaca
 *
 * example2
 * input:	aaabb
 * output:	ababa
 *
 * example3:
 * input:	aaaabc
 * output:	Not possible
 * */


struct char_freq {
	int freq;
	char ch;
};

char *rearrange(char *input)
{
	//TODO struct binary_heap *max_heap;
	struct char_freq *cf;
	struct char_freq *prev;
	struct char_freq dummy = {-1, 0};
	char *output;
	int freq[256] = {0};
	int len;
	int i;

	len = strlen(input);
	/*compute frequency of each char*/
	for (i = 0; i < len; i++) {
		(freq[(int)input[i]])++;
	}
	
	//TODO binary_heap_init(&max_heap, 256, MAX_HEAP, compare_cb);

	/*add each unique char with its frequency to max heap*/
	for (i = 0; i < 256; i++) {
		if (freq[i]) {
			cf = malloc(sizeof(*cf));
			assert(cf);
			cf->freq = freq[i]; 
			cf->ch = (char)i;
			//TODO binary_heap_insert(max_heap, cf);
		}
	}

	output = malloc(sizeof(*output) *  len);
	i = 0;
	prev = &dummy;
	while (0 /*TODO !binary_heap_is_empty(max_heap)*/) {
		//TODO cf = binary_heap_remove(max_heap);
		/*prev->ch will be 0 for 1st iteration of this loop*/
		if (prev->ch && prev->ch == cf->ch) {
			/*solution not possible*/
			strncpy(output, "X", len);
			break;
		}
		output[i++] = cf->ch;
		cf->freq--;
		if (prev->ch && prev->freq) {
			//TODO binary_heap_insert(max_heap, prev);
		}
		prev = cf;
	}
	return output;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input string>\n", argv[0]);
		return 1;
	}
	printf("input:\t%s\n", argv[0]);
	printf("output:\t%s\n", rearrange(argv[0]));
	return 0;
}
