#include<stdio.h>
#include<unistd.h>

int main()
{
	long total_available_ram =sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE) ;
	printf("RAM = [%ld]\n", total_available_ram);
	return 0;
}

