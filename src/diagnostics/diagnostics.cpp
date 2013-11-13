#include "../../lib/perseus.h"
#include "../../lib/diagnostics.h"
#include "../../lib/port_designations.h"

#define MAX 255

using namespace std;

/**
 * Info and Diagnostics 
 * 
 * - Battery Voltage
 * - Sea temp 
 * - Lon/Lat Coordinates
 */

void *diagnostics_handler(void *data) {
	char buf[MAX];
	int sockfd = (*(int*)data);
	char *pch;
	int n;

	while((n=read(sockfd, buf, MAX)) > 0) {
		/* try to write to the socket to make sure its still alive */
		strip_newline(buf);
		pch = strtok(buf, " ,");
		if(pch != NULL) {
			if(strcmp(pch, "/temp") == 0) {
				send_response(&water_temp, sockfd);
			}
			if(strcmp(pch, "/position") == 0) {
				send_response(&current_position, sockfd);
			}
			if(strcmp(pch, "/battery") == 0) {
				send_response(&battery_status, sockfd);
			}
			if(strcmp(pch, "/thrust") == 0) {
				send_response(&get_vehicle_thrust, sockfd);
			}
			if(strcmp(pch, "/orientation") == 0) {
				send_response(&get_vehicle_orientation, sockfd);
			}
			if(strcmp(pch, "/info") == 0) {
				send_response(&get_vehicle_info, sockfd);
			}
		}
		//strcpy(buf, "mission_control> ");
		//if(write(sockfd, buf, strlen(buf)+1) < 0) break;
				
	}
	printf(YELLOW "[DEBUG] CONNECTION CLOSED\n" RESET);
	close(sockfd);
	free(data);
}

void *init_diagnostics(void *data) {
	/* Initialize sockets */
	int *newsockfd;
	int sockfd, clilen, pid;
	char buf[MAX];
		
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	pthread_t worker;
	memset(&serv_addr, 0, sizeof(serv_addr));
	

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf(RED "[ERROR] Diagnostics: error creating the socket\n" RESET);
		exit(1);
	}

	// bind the socket to a port
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(DIAGNOSTICS_PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	/* Bind the TCP socket and start listening for connections */
	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	listen(sockfd, 10);

	printf(GREEN "[DEBUG] Diagnostics server listening on 0.0.0.0:%d\n" RESET, DIAGNOSTICS_PORT);

	/* Handle incomming connections */
	while(1) {
		// handle a new connection
		newsockfd = (int*)malloc(sizeof(int));
		if((*newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen)) < 0) {
			printf(RED "[ERROR] Diagnostics: An error occured on socket accept\n" RESET);
		}

		pthread_create(&worker, NULL, &diagnostics_handler, (void*)newsockfd);	
		pthread_join(worker, NULL);
	}
	close(sockfd);
}

