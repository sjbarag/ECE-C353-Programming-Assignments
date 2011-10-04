#include <stdio.h>
#include <sys/mman.h>
//#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
//#include <time.h>

#define SO_PATH "/sjb89_hw1-5"
#define MAX_MESSAGE_LENGTH 50

struct msg_s
{
	char content[MAX_MESSAGE_LENGTH];
};

int main(int argc, char **argv[])
{
	int fd;
	int shared_seg_size = (sizeof(struct msg_s)); // We want a shared segment capable of storing one message
	struct msg_s *shared_msg;   // shared segment and head of the messages list

	fd = shm_open(SO_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
	if (fd < 0)
	{
		printf("Error in shm_open()!");
		exit(-1);
	}
	printf("Success in creating shared memory object at /dev/shm%s\n", SO_PATH);

}
