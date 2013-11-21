#include "../../lib/linear_actuator.h"
#include "../../lib/perseus.h"
#include "../extern.h"

#define MAX 255

int la_fd = -1; /* global arduino communication fd */

void connect_to_la();

int leds_prev;
int laser_prev;
int la_prev;

void *init_la(void *data) {
	char buf[MAX];
	
	/* not connected to arduino, attempt to reconnect */
	if(la_fd < 0) connect_to_la();

	/* open up a serial connection to the thruster controller arduino A0 */
	while(1) {

		/* this thread will block if the mutex is currently locked */
		pthread_mutex_lock(&thrust_mutex);
		/* only update thrust command if there is a change */
		if(leds_prev 	!= linear_actuator_state->leds_on 	||
		   laser_prev 	!= linear_actuator_state->laser_on 	||
		   la_prev 	!= linear_actuator_state->la_on)
		{
			/* thrust has changed, update Arduino command */
			printf(YELLOW "[DEBUG] {Updating Laser/LED/LA command to Arduino} (%d,%d,%d)\n" RESET, 
				linear_actuator_state -> leds_on,
				linear_actuator_state -> laser_on,
				linear_actuator_state -> la_on);

			sprintf(buf, "%d,%d", linear_actuator_state->leds_on, linear_actuator_state -> la_on); 
	
			/* update new prev thrust values */
			leds_prev = linear_actuator_state -> leds_on;
			laser_prev = linear_actuator_state -> leds_on;
			la_prev = linear_actuator_state -> la_on;
			
			if(write(la_fd, buf, strlen(buf)) < 0) {
				printf(RED "[ERROR] Failed to write '%s' to LA arduino" RESET, buf);
				close(la_fd);
				connect_to_la();
			}

		}
		pthread_mutex_unlock(&thrust_mutex);
		usleep(150000); /* wait 50ms before updating new thrust */
	}
}

void connect_to_la() {
	char buf[MAX];

	strcpy(buf, "/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_7523230323535140F021-if00");

	la_fd = open(buf, O_RDWR | O_NOCTTY | O_SYNC);
	if(la_fd < 0) {
		printf(RED "[ERROR] Failed to open /dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_7523230323535140F021-if00\n" RESET);
	}
	set_interface_attribs(la_fd, B9600, 0);
	set_blocking(la_fd, 0);
}

