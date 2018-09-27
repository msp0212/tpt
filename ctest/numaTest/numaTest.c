#include<stdio.h>
#include<stdlib.h>
#include<numa.h>

int main()
{
	int num_numa_nodes;
	if(numa_available() < 0)
	{
		printf("System does not support NUMA api !!!\n");
		exit(1);
	}
	printf("System supports NUMA api !!!\n");
	num_numa_nodes = numa_max_node();
	printf("No of numa nodes available = [%d]\n", num_numa_nodes);
	return 0;
}
