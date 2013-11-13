#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../lib/perseus.h"
#include "../lib/port_designations.h"
#include "extern.h"

void *init_gps_fix(void *data) {

	printf(YELLOW "[DEBUG] Init GPS fix" RESET "\n");

	int timeout = 5;
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];

	/*char *hostname = "192.168.10.91";
	portno = 1477;*/

	while (1){
		printf(YELLOW "[DEBUG] GPS: connecting to %s:%i" RESET "\n",BEAGLEBONE_SURFACE_IP,BEAGLEBONE_SURFACE_PORT);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			printf(RED "[DEBUG] GPS: ERROR opening socket" RESET "\n");
			sleep(timeout);
			continue;
		}
		server = gethostbyname(BEAGLEBONE_SURFACE_IP);
		if (server == NULL) {
			printf(RED "[DEGUB] GPS: ERROR, no such host" RESET "\n");
			sleep(timeout);
			continue;
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port = htons(BEAGLEBONE_SURFACE_PORT);

		if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
		{
			printf(RED "[DEBUG] GPS: ERROR connecting" RESET "\n");
			sleep(timeout);
			continue;
		}

		while (1){
			n = write(sockfd,"\n",1);
			if (n < 0) {
				printf(RED "[DEBUG] GPS: ERROR writing to socket" RESET "\n");
				break;
			}
			bzero(buffer,256);
			n = read(sockfd,buffer,255);
			/*printf("Read %s\n", buffer);
			char tok[280];

			strncpy(buffer,tok,255);
			
			pthread_mutex_lock(&position_mutex);
				printf("%s\n", strtok(tok, ","));
				printf("%s\n", strtok(NULL, ","));
			//	vehicle_position -> lat 			= strtod(strtok(buffer, ","),NULL);
			//	vehicle_position -> lon 			= strtod(strtok(NULL, ","),NULL);
			//	vehicle_position -> sv_in_view 		= atoi(strtok(NULL, ","));
			//	vehicle_position -> sv_in_use 		= atoi(strtok(NULL, ","));
			//	vehicle_position -> horizontal_dil 	= atof(strtok(NULL, ",")); 
			pthread_mutex_unlock(&position_mutex);*/
			if (n < 0) {
				printf(RED "[DEBUG] GPS: ERROR reading from socket" RESET "\n");
				break;
			}
			printf(GREEN "[DEBUG] GPS: %s" RESET "",buffer);
			sleep(1);
		}
		close(sockfd);
	}
}
