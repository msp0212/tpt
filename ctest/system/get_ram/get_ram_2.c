#include <stdio.h>
#include <unistd.h>


int main(void)
{
    printf("total ram = %llu\n", sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE));
    return 0;
}
