#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define DEVICE_FILE_NAME "/dev/MyBufferedMem"
#define CH_READ_BUFFER_SIZE 0
int main(int argc, char **argv)
{
	int device;
	int n = atoi(argv[1]);
	unsigned char rbuf[n];

	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY);
	if (device >- 0)
	{
		
		printf("new read buffer size is..: %d\n", n);
		ioctl(device, CH_READ_BUFFER_SIZE ,n);
		

	
		read(device, rbuf, sizeof(rbuf));
		printf("Read value is %s\n", rbuf);

	}
	else
		perror("Device file open fail...\n");

	close(device);

	return 	0;
}
