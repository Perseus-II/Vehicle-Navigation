#include "../../lib/perseus.h"
#include "../../lib/thruster_control.h"
#include "../extern.h"

#define MAX 256

float sp_prev;
float ss_prev;
float ha_prev;
float hb_prev;

int arduino_fd = -1; /* global arduino communication fd */

void connect_to_arduino();

void *init_thruster_control(void *data) {
	char buf[MAX];
	
	/* not connected to arduino, attempt to reconnect */
	if(arduino_fd < 0) connect_to_arduino();

	/* open up a serial connection to the thruster controller arduino A0 */
	while(1) {

		/* this thread will block if the mutex is currently locked */
		pthread_mutex_lock(&thrust_mutex);
		/* only update thrust command if there is a change */
		if(sp_prev != vehicle_thrust->surge_port 	||
		   ss_prev != vehicle_thrust->surge_starboard 	||
		   ha_prev != vehicle_thrust->heave_a		||
		   hb_prev != vehicle_thrust->heave_b	)
		{
			/* thrust has changed, update Arduino command */
			printf(YELLOW "[DEBUG] {Updating Thrust to Arduino} (%.2f,%.2f,%.2f,%.2f)\n" RESET, 
				vehicle_thrust->surge_port,
				vehicle_thrust->surge_starboard,
				vehicle_thrust->heave_a,
				vehicle_thrust->heave_b);

			sprintf(buf, "%d,%d,%d,%d", int(vehicle_thrust->surge_port),
						    int(vehicle_thrust->surge_starboard),
						    int(vehicle_thrust->heave_a), 
						    int(vehicle_thrust->heave_b));
	
			/* update new prev thrust values */
			sp_prev = vehicle_thrust->surge_port;
			ss_prev = vehicle_thrust->surge_starboard;
			ha_prev = vehicle_thrust->heave_a;
			hb_prev = vehicle_thrust->heave_b;
			
			if(write(arduino_fd, buf, strlen(buf)) < 0) {
				printf(RED "[ERROR] Failed to write '%s' to /dev/bttyACM0 (Arduino0)\n" RESET, buf);
				close(arduino_fd);
				connect_to_arduino();
			}

		}
		pthread_mutex_unlock(&thrust_mutex);
		usleep(150000); /* wait 50ms before updating new thrust */
	}
}

void connect_to_arduino() {
	char buf[MAX];

	strcpy(buf, "/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_75232303235351A0E130-if00");

	arduino_fd = open(buf, O_RDWR | O_NOCTTY | O_SYNC);
	if(arduino_fd < 0) {
		printf(RED "[ERROR] Failed to open /dev/ttyACM0\n" RESET);
	}
	set_interface_attribs(arduino_fd, B115200, 0);
	set_blocking(arduino_fd, 0);
}

