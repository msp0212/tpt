#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILEPATH "./sample.txt"
#define NUMINTS  (5)
#define FILESIZE (NUMINTS * sizeof(int))

int main(int argc, char *argv[])
{
				int i;
				int fd;
				char *map;  /* mmapped array of char */

				fd = open(FILEPATH, O_RDWR);
				if (fd == -1) {
								perror("Error opening file for reading");
								exit(EXIT_FAILURE);
				}

				map = mmap(0, FILESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
				if (map == MAP_FAILED) {
								close(fd);
								perror("Error mmapping the file");
								exit(EXIT_FAILURE);
				}

				/* Read the file char-by-char from the mmap
				 **/
				for (i = 0; i <NUMINTS; ++i) {
								printf("%d: %c\n", i, map[i]);
				}
				/*change the character value*/	
				map[2]='9';
				if (munmap(map, FILESIZE) == -1) {
								perror("Error un-mmapping the file");
				}
				close(fd);
				return 0;
}
