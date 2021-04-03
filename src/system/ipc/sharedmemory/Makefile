.PHONY : clean
all : shm_producer shm_consumer

shm_producer : shm_producer.c 
	gcc -Wall -Werror -o $@ shm_producer.c -lrt -pthread

shm_consumer : shm_consumer.c 
	gcc -Wall -Werror -o $@ shm_consumer.c -lrt -pthread

clean :
	-@rm -f shm_producer shm_consumer
