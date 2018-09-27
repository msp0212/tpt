#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>
#include<string.h>
#include<sys/time.h>

#define HT_SIZE 1000000
struct header_node {
	char *name;
	struct header_node *next;
};

struct header_table {
	struct header_node *head;
	int node_count;
};

long long ustime(void);
int ht_lookup(struct header_table *ht, int index, char *key);
int list_search(struct header_node *head, char *key);
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed);
int ht_insert(struct header_table **ht, int index, char *key);
int list_add(struct header_node **head, char *key);
void print_ht(struct header_table *ht);
void print_list(struct header_node *head);

int main()
{
	int c;
	int i = 0;
	char *str = NULL;
	unsigned int hv = 0;
	unsigned int hvf = 0;
	int start_search = 0;
	struct header_table *ht = NULL;
	long long t1,t2;

	t1 = ustime();
	str = malloc(128);
	ht = calloc(HT_SIZE, sizeof(struct header_table));
	
	while ((c = getchar()) != EOF) {
		if (c == '\n') {
			str[i] = '\0';
		//	printf("[%s] ", str);
			i = 0;
			if(strcmp(str,"|") == 0) {
				start_search = 1;
				continue;
			}
			hvf = murmur3_32(str, strlen(str), 5341);
			//hv = hvf & 99999;
			hv = hvf % HT_SIZE;
			//hv = hvf & mask;
		//	printf("%u %u\n", hvf, hv);
			if(start_search) {
				if (ht_lookup(ht, hv, str) == 1) 
		//			printf("Hit\n");
					;
				else 
		//			printf("Miss\n");
					;
			} else {
				ht_insert(&ht, hv, str);
			}
		} else {
			str[i] = c;
			i++;
		}
	}
	t2 = ustime();
//	printf("time taken = %lld\n", t2 - t1);
//	printf("##### Hash Table ######\n");
	print_ht(ht);
	free(str);
	return 0;
}

long long ustime(void) {
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec)*1000000;
	ust += tv.tv_usec;
	return ust;
}

int ht_insert(struct header_table **ht, int index, char *key) 
{
	if ((*ht)[index].node_count > 0) {
//		printf("Collision for key [%s] index [%d]\n", key, index);
	}
	list_add(&((*ht)[index].head), key);
	(*ht)[index].node_count++;
	return 0;
}
int ht_lookup(struct header_table *ht, int index, char *key)
{
	int ret = 0;
	if (ht[index].node_count == 0)
	{
		ret = 0;
	} else if (ht[index].node_count == 1) {
		ret = 1;
	} else {
		if (list_search(ht[index].head, key) == 1) 
			ret = 1;
		else 
			ret = 0;
	}
	return ret;
}
int list_search(struct header_node *head, char *key)
{
	int ret = 0;
	struct header_node *tmp = head;
	while (tmp != NULL) {
		if (strcmp(tmp->name, key) == 0) {
			ret = 1;
			break;
		}
		tmp = tmp->next;
	}
	return ret;
}
int list_add(struct header_node **head, char *key) 
{
	struct header_node *tmp = NULL;

	tmp = malloc(sizeof(struct header_node));
	tmp->name = strdup(key);
	tmp->next = NULL;

	if (*head == NULL) {
		*head = tmp;
	} else {
		tmp->next = *head;
		*head = tmp;
	}
	return 0;
}
void print_ht(struct header_table ht[]) 
{
	int i = 0;
	for (i = 0; i < HT_SIZE; i++) {
		printf("%d ", i+1);
	#if 0
		if(ht[i].head) 
			print_list(ht[i].head);
		else
			printf("null ");
	#endif
		printf("%d\n", ht[i].node_count);
	}
}

void print_list(struct header_node *head)
{
	struct header_node *tmp = head;
	while (tmp != NULL) {
		if (tmp->name) 	{
			printf("%s ", tmp->name);
		} else {
			printf("null_name ");
		}
		tmp = tmp->next;
	}
}
uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed)
{
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	uint32_t hash = seed;

	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	for (i = 0; i < nblocks; i++) {
		uint32_t k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
		case 3:
			k1 ^= tail[2] << 16;
		case 2:
			k1 ^= tail[1] << 8;
		case 1:
			k1 ^= tail[0];

			k1 *= c1;
			k1 = (k1 << r1) | (k1 >> (32 - r1));
			k1 *= c2;
			hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}
