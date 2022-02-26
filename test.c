#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#define DEVICE_FILE_NAME "/dev/MyBufferedMem"

int main(int argc, char **argv)
{
	int device;
	char wbuf[16] = "abcdefghigklmnop";
	unsigned char rbuf[8];
	//int n = atoi(argv[1]);

	device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY);
	if (device >- 0)
	{
		//printf("ioctl is called: %d\n", n);
		//ioctl(device, n);

		write(device, wbuf, sizeof(wbuf));
		//printf("write value is %s\n", wbuf);

		read(device, rbuf, sizeof(rbuf));
		printf("Read value is %s\n", rbuf);
	}
	else
		perror("Device file open fail...\n");

	close(device);

	return 	0;
}
