#include <stdio.h>
#include <stdint.h>


void print_setbit_inx(uint32_t num)
{
	uint32_t tmp; 
	uint8_t i = 0;

	while (num) {
		tmp = num & (-num);
		num = num ^ tmp; 
		printf("num = %x, tmp = %x\n", num, tmp);
		i++;
	}
	printf("i = %d\n", i);
}

#define BYTE_TO_BINARY_FORMAT "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
	(byte & 0x80) ? '1' : '0', \
	(byte & 0x40) ? '1' : '0', \
	(byte & 0x20) ? '1' : '0', \
	(byte & 0x10) ? '1' : '0', \
	(byte & 0x08) ? '1' : '0', \
	(byte & 0x04) ? '1' : '0', \
	(byte & 0x02) ? '1' : '0', \
	(byte & 0x01) ? '1' : '0'


void print_binary(uint8_t num)
{
	printf(BYTE_TO_BINARY_FORMAT"\n", BYTE_TO_BINARY(num));
}

int main()
{
	uint32_t flags = 0x80808080;
	int8_t num = -64;

	print_setbit_inx(flags);
	
	print_binary(num);

	num = num >> 1;
	print_binary(num);
	num = num >> 1;
	print_binary(num);

	return 0;
}



