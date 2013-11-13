#include <unistd.h>
#include <stdlib.h>
#include "../lib/perseus.h"

void init_camera_feed(int dev, int port) {
	int pid;
	char buf[256];

	if(dev == -1) dev = 0;

	if((pid = fork()) < 0) {
		fprintf(stderr, RED "[ERROR] A fork error occured" RESET);
	}
	if(pid == 0) {
		/* child process */
		sprintf(buf, "/usr/sbin/framegrabber -d /dev/video%d -f mjpeg -W 1024 -H 768 -I 10 -o -c -1 -p %d", dev, port);
		printf(YELLOW "[DEBUG] Starting camera feed on device /dev/video%d [%d]" RESET "\n", dev, port);
		printf(YELLOW "[DEBUG] Framegrabber: %s" RESET "\n", buf);
		system(buf);
	} else {
		return;
	}

}
