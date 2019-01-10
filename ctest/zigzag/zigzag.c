#include <stdio.h>
#include <stdint.h>

uint64_t zigzag_encode_int64(int64_t num)
{
	return ((num << 1) ^ (num >> 63));
}

int64_t zigzag_decode_uint64(uint64_t num) 
{
	return ((num >> 1) ^ (-(num & 0x1)));
}

int main(int argc, char *argv[])
{
	int64_t num_arr[16] = { -7, -6, -5, -4, -3, -2, -1, 
				0, 1, 2, 3, 4, 5, 6, 7};
	uint64_t zigzag_arr[16];
	int i;

	for (i = 0; i < 15; i++) {
		uint64_t tmp;
		tmp = zigzag_encode_int64(num_arr[i]);
		zigzag_arr[i] = tmp;
		printf("num [%ld] zigzag [%lu]\n", num_arr[i], tmp);
	}

	printf("\n\n");
	for (i = 0; i < 15; i++) {
		int64_t tmp;
		tmp = zigzag_decode_uint64(zigzag_arr[i]);
		printf("zigizag [%lu] num [%ld]\n", zigzag_arr[i], tmp);
	}
	return 0;
}
