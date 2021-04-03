#include <stdio.h>
#include <stdint.h>
#include <sys/sysinfo.h>

int main(void)
{
    struct sysinfo info;

    if (sysinfo(&info) < 0) {
        return 1;
    }
    printf("total ram = %llu\n", info.totalram * info.mem_unit);
    return 0;
}
