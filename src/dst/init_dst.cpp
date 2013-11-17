#include "../../lib/dst.h"
#include "../../lib/perseus.h"
#include "../extern.h"

#define MAX 255

using namespace std;

void connect_to_dst();

int dst_fd = -1;

void *init_dst(void *data) {
	char c;
	char line[MAX];
	char *pch;
	int i;
	int n;
	float depth_ft, depth_m, temp;

	while(1) {
		if(dst_fd < 0) connect_to_dst();
	
		i = 0;
		bzero(line,MAX);
		while((n=read(dst_fd,&c,1)) > 0) {
			if(c == '\n') {
				/* line complete, do something with it */
				pch = strtok(line, ",");		
				if(pch != NULL) {
					/* $SDDBT,1.5,f,0.4,M,0.2,F*04 */
					if(strcmp(pch, "$SDDBT") == 0) {
						/* depth from bottom */
						pch = strtok(NULL, ",");
						depth_ft = atof(pch);	
						pch = strtok(NULL, ",");
						pch = strtok(NULL, ",");
						depth_m = atof(pch);	
						pthread_mutex_lock(&dst_mutex);
						vehicle_dst -> depth_ft = depth_ft;
						vehicle_dst -> depth_m = depth_m;
						pthread_mutex_unlock(&dst_mutex);
					} else if(strcmp(pch, "$YXMTW") == 0) {
						/* water temperature */
						/* $YXMTW,28.1,C*19 */
						pch = strtok(NULL, ",");
						temp = atof(pch);
						pthread_mutex_lock(&dst_mutex);
						vehicle_dst -> temperature = temp;
						pthread_mutex_unlock(&dst_mutex);
					}
				}
				break;
			}
			line[i++] = c;	
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
	sleep(2);
}
