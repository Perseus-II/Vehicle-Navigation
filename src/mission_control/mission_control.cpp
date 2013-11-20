#include "../../lib/perseus.h"
#include "../../lib/mission_control.h"
#include "../../lib/port_designations.h"

#define MAX 1024

using namespace std;

/**
 * Mission Control
 */

void *mission_control_handler(void *data) {
	char buf[MAX];
	char *pch;

	int sockfd = (*(int*)data);
	float surge_port;
	float surge_starboard;
	float heave_a;
	float heave_b;
	float pitch, roll, yaw;	
	float desired_depth;
	int n;
	int mode;
	
	float kp, ki, kd;
	
	printf(YELLOW "[DEBUG] Mission control: New connection sockfd = %d\n" RESET, sockfd);	

	//strcpy(buf, "mission_control> ");
	//write(sockfd, buf, strlen(buf)+1);

	while((n=read(sockfd, buf, MAX)) > 0) {
		printf("Read: %s\n", buf);
		/* try to write to the socket to make sure its still alive */
		strip_newline(buf);
		pch = strtok(buf, " ,");
		if(pch != NULL) {
			if(strcmp(pch, "/set_thrust") == 0) {
				/* set thrust manually */
				surge_port = atof(strtok(NULL, " ,"));	
				surge_starboard= atof(strtok(NULL, " ,"));	
				heave_a = atof(strtok(NULL, " ,"));	
				heave_b = atof(strtok(NULL, " ,"));	
				set_thrust(surge_port, surge_starboard, heave_a, heave_b);		
			}
			if(strcmp(pch, "/set_orientation") == 0) {
				/* set thrust manually */
				pitch = atof(strtok(NULL, " ,"));	
				roll = atof(strtok(NULL, " ,"));	
				yaw = atof(strtok(NULL, " ,"));	
				set_orientation(pitch, roll, yaw);
			}
			if(strcmp(pch, "/set_mode") == 0) {
				/* set thrust manually */
				mode = atoi(strtok(NULL, " ,"));	
				set_vehicle_mode(mode);
			}
			if(strcmp(pch, "/set_pid_values") == 0) {
				/* set thrust manually */
				kp = atof(strtok(NULL, " ,"));	
				ki = atof(strtok(NULL, " ,"));	
				kd = atof(strtok(NULL, " ,"));	
				set_pid_values(kp,ki,kd);
			}
			if(strcmp(pch, "/set_current_orientation") == 0) {
				/* set thrust manually */
				set_current_orientation();
			}
			if(strcmp(pch, "/set_depth") == 0) {
				/* set thrust manually */
				desired_depth = atof(strtok(NULL, " ,"));
				set_desired_depth(desired_depth);
				printf("Set desired depth = %f\n", desired_depth);
			}
		}
		//strcpy(buf, "mission_control> ");
		//if(write(sockfd, buf, strlen(buf)+1) < 0) break;
				
	}
	printf(YELLOW "[DEBUG] CONNECTION CLOSED\n" RESET);
	close(sockfd);
	free(data);
}



void *init_mission_control(void *data) {
	/* Initialize sockets */
	int *newsockfd;
	int sockfd, clilen, pid;
	char buf[MAX];
		
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;

	memset(&serv_addr, 0, sizeof(serv_addr));
	

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf(RED "[ERROR] Misson Control: error creating the socket\n" RESET);
		exit(1);
	}

	// bind the socket to a port
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(MISSION_CONTROL_PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	/* Bind the TCP socket and start listening for connections */
	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	listen(sockfd, 10);

	printf(GREEN "[DEBUG] Mission control server listening on 0.0.0.0:%d\n" RESET, MISSION_CONTROL_PORT);

	/* Handle incomming connections */
	while(1) {
		pthread_t worker;
		// handle a new connection
		newsockfd = (int*)malloc(sizeof(int));
		if((*newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen)) < 0) {
			printf(RED "[ERROR] Mission Control: An error occured on socket accept\n" RESET);
		}

		pthread_create(&worker, NULL, &mission_control_handler, (void*)newsockfd);	
		pthread_join(worker, NULL);
	}
	close(sockfd);
}

