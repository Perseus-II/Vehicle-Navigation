#include "../../lib/dst.h"
#include "../../lib/perseus.h"

#define MAX 255

using namespace std;

void connect_to_dst();

int dst_fd = -1;

void *init_dst(void *data) {
	char buf[MAX];
	int n;

	while(1) {
		if(dst_fd < 0) connect_to_dst();
	
		if((n=read(dst_fd, buf, MAX)) > 0) {
			printf("Read %s from DST\n", buf);
		}	
	}
}

void connect_to_dst() {
	char buf[MAX];

	strcpy(buf, "/dev/serial/by-id/usb-Actisense_USG-1_2081F-if00-port0");

	dst_fd = open(buf, O_RDWR | O_NOCTTY | O_SYNC);
	if(dst_fd < 0) {
		printf(RED "[ERROR] Failed to open /dev/serial/by-id/usb-Actisense_USG-1_2081F-if00-port0\n" RESET);
	}
	set_interface_attribs(dst_fd, B4800, 0);
	set_blocking(dst_fd, 1);
	printf(YELLOW "[DEBUG] DST: Connected to Actisense DST" RESET "\n");
}
