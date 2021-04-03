#include <stdio.h>
#include <strings.h>
#include <stdint.h>

#define NUM 0x12345678
//#define NUM 0x112244FF

uint8_t lut[32] = {
	1, 2, 3, 4, 5, 6, 7, 8,
	9, 10, 11, 12, 13 ,14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24,
	25, 26, 27, 28, 29, 30, 31, 32
};

uint8_t inx_tbl[128];

void init_bit_inx_table(void) 
{
	bzero(inx_tbl, sizeof(inx_tbl));
	inx_tbl[1] = 0;
	inx_tbl[2] = 1;
	inx_tbl[4] = 2;
	inx_tbl[8] = 3;
	inx_tbl[16] = 4;
	inx_tbl[32] = 5;
	inx_tbl[64] = 6;
	inx_tbl[128] = 7;
}

#define MASK 0xff


int main()
{
	uint32_t num = NUM;
	uint8_t *byte = NULL;
	uint8_t temp = 0;
	int i = 0;
	init_bit_inx_table();

	printf("---%x---\n", num);
	byte = (uint8_t *)(&num);
	while (i < sizeof(num)) {
		printf("byte = %x\n", *byte);
		while (*byte) {
			temp = (*byte) & -(*byte);
			printf("%x %x\n", *byte, -(*byte));
			*byte ^= temp;

			printf("%x %d %d\n", *byte, temp, lut[inx_tbl[i*8 + temp]]);
			printf("---%d %d---\n", i, lut[i*8 + inx_tbl[temp]]);
		}
		byte++;
		i++;
	}
}
