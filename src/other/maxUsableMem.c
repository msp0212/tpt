#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[]){
        long long oneHundredMiB=100*1048576;
        long long maxMemMiB=0;
        void *memPointer = NULL;
	void *tmp;
        do{
                if(memPointer != NULL){
                        printf("Max Tested Memory = %lld\n",maxMemMiB);
                        memset(memPointer,0,maxMemMiB);
                        free(memPointer);
                }
                maxMemMiB+=oneHundredMiB;

                memPointer  = calloc(1, maxMemMiB);
		if(tmp == NULL) {
			perror("malloc");
		}
        }while(memPointer != NULL);
        printf("Max Usable Memory aprox = %lld\n",maxMemMiB-oneHundredMiB);

	sleep(60);
        return 0;
}
