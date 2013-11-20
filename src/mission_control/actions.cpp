#include "../../lib/mission_control.h"
#include "../../lib/perseus.h"
#include "../extern.h"

#define MAX 64

using namespace std;

char *set_thrust(float surge_port, float surge_starboard, float heave_a, float heave_b) {
	/* update the global thrust vector */
	int vmode;
	pthread_mutex_lock(&mode_mutex);	
	vmode = vehicle_mode;
	pthread_mutex_unlock(&mode_mutex);	

	if(vmode > 0) {
		pthread_mutex_lock(&thrust_mutex);
			vehicle_thrust -> surge_port 		= surge_port;
			vehicle_thrust -> surge_starboard 	= surge_starboard;
			vehicle_thrust -> heave_a 		= heave_a;
			vehicle_thrust -> heave_b 		= heave_b;
		pthread_mutex_unlock(&thrust_mutex);
	}
}

char *set_orientation(float pitch, float roll, float yaw) {
	pthread_mutex_lock(&desired_orientation_mutex);
	desired_vehicle_orientation -> pitch = pitch;
	desired_vehicle_orientation -> roll = roll;
	desired_vehicle_orientation -> yaw = yaw;
	pthread_mutex_unlock(&desired_orientation_mutex);
}

char *set_vehicle_mode(int mode) {
	pthread_mutex_lock(&mode_mutex);
	vehicle_mode = mode;
	pthread_mutex_unlock(&mode_mutex);
}

char *set_pid_values(float kp, float ki, float kd) {
	pthread_mutex_lock(&pid_values_mutex);
	vehicle_pid_values -> kp = kp;
	vehicle_pid_values -> ki = ki;
	vehicle_pid_values -> kd = kd;
	pthread_mutex_unlock(&pid_values_mutex);
}

char* set_current_orientation() {
	float pitch, roll, yaw;
	pthread_mutex_lock(&orientation_mutex);
	pitch  = vehicle_orientation -> pitch;
	roll  = vehicle_orientation -> roll;
	yaw  = vehicle_orientation -> yaw;
	pthread_mutex_unlock(&orientation_mutex);

	pthread_mutex_lock(&initial_orientation_mutex);
	vehicle_initial_orientation -> pitch = pitch; 
	vehicle_initial_orientation -> roll = roll; 
	vehicle_initial_orientation -> yaw = yaw; 
	pthread_mutex_unlock(&initial_orientation_mutex);

	printf(YELLOW "[DEBUG] Updating current orientation to %f,%f,%f\n" RESET, pitch, roll, yaw);
}

char *set_desired_depth(float depth) {
	pthread_mutex_lock(&desired_orientation_mutex);
	desired_vehicle_orientation -> depth = depth;
	pthread_mutex_unlock(&desired_orientation_mutex);
}
