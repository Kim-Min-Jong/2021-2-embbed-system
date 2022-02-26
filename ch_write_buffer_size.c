#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/ioctl.h>

#define DEVICE_FILE_NAME "/dev/MyBufferedMem"
#define CH_WRITE_BUFFER_SIZE 1
int main(int argc, char **argv)
{
	int device;
	char wbuf[16] = "abcdefghijkimnop";

	int n = atoi(argv[1]);

	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY);
	if (device >- 0)
	{
		
		printf("buffer size is..: %d\n", n);
		ioctl(device, CH_WRITE_BUFFER_SIZE ,n);
		
		write(device, wbuf, sizeof(wbuf));

	}
	else
		perror("Device file open fail...\n");

	close(device);

	return 	0;
}
