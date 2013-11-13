#include "../../lib/diagnostics.h"
#include "../../lib/perseus.h"
#include "../extern.h"

using namespace std;


char *water_temp() {
	char *msg = (char*)malloc(255*sizeof(char));
	strcpy(msg, "19.5C");
	return msg;
}

char *current_position() {
	/* Get the current longitude and latitude for the vehicle 
	 * make sure to use mutexes on the coordinatees */
	char *msg = (char*)malloc(255*sizeof(char));

	pthread_mutex_lock(&position_mutex);
	sprintf(msg, "(%.2f,%.2f,%.2f)", 
		vehicle_position -> x,
		vehicle_position -> y,
		vehicle_position -> z);	
	pthread_mutex_unlock(&position_mutex);	
	return msg;
}

char *battery_status() {
	char *msg = (char*)malloc(255*sizeof(char));
	strcpy(msg, "23.2V");
	return msg;
}

char *get_vehicle_thrust() {
	char *msg = (char*)malloc(255*sizeof(char));

	pthread_mutex_lock(&thrust_mutex);
	/* critical section (thrust_mutex) */
	sprintf(msg,"(%.2f,%.2f,%.2f,%.2f)",
			vehicle_thrust -> surge_port,
			vehicle_thrust -> surge_starboard,
			vehicle_thrust -> heave_a,
			vehicle_thrust -> heave_b);
	/* end critical section (thrust_mutex) */
	pthread_mutex_unlock(&thrust_mutex);
	return msg;
}
char *get_vehicle_orientation() {
	char *msg = (char*)malloc(255*sizeof(char));

	pthread_mutex_lock(&orientation_mutex);
	/* critical section (thrust_mutex) */
	sprintf(msg,"%.6f,%.6f,%.6f",
			vehicle_orientation -> pitch,
			vehicle_orientation -> roll,
			vehicle_orientation -> yaw);
	/* end critical section (thrust_mutex) */
	pthread_mutex_unlock(&orientation_mutex);
	return msg;
}

char *get_vehicle_info() {
	char *msg = (char*)malloc(1024*sizeof(char));
	
	/* return a long string containing all available vehicle information */
	/*
		position_lat,
		position_lon,
		thrust_1,
		thrust_2,
		thrust_3,
		thrust_4,
		orientation_pitch,
		orientation_roll,
		orientation_yaw,
		desired_orientation_pitch,
		desired_orientation_roll,
		desired_orientation_yaw,
	*/
	double position_lat, position_lon;
	float thrust_1, thrust_2, thrust_3, thrust_4;
	float orientation_pitch, orientation_roll, orientation_yaw;
	float desired_orientation_pitch, desired_orientation_roll, desired_orientation_yaw;
	int mode;
	float kp, ki, kd;

	pthread_mutex_lock(&position_mutex);
	position_lat = vehicle_position -> lat;
	position_lon = vehicle_position -> lon;
	pthread_mutex_unlock(&position_mutex);
 
	pthread_mutex_lock(&thrust_mutex);
	thrust_1 = vehicle_thrust -> surge_port; 
	thrust_2 = vehicle_thrust -> surge_starboard; 
	thrust_3 = vehicle_thrust -> heave_a; 
	thrust_4 = vehicle_thrust -> heave_b; 
	pthread_mutex_unlock(&thrust_mutex);

	pthread_mutex_lock(&orientation_mutex);
	orientation_pitch = vehicle_orientation -> pitch;
	orientation_roll = vehicle_orientation -> roll;
	orientation_yaw = vehicle_orientation -> yaw;
	pthread_mutex_unlock(&orientation_mutex);

	pthread_mutex_lock(&desired_orientation_mutex);
	desired_orientation_pitch = desired_vehicle_orientation -> pitch;
	desired_orientation_roll = desired_vehicle_orientation -> roll;
	desired_orientation_yaw = desired_vehicle_orientation -> yaw;
	pthread_mutex_unlock(&desired_orientation_mutex);

	pthread_mutex_lock(&mode_mutex);
	mode = vehicle_mode;
	pthread_mutex_unlock(&mode_mutex);

	pthread_mutex_lock(&pid_values_mutex);
	kp = vehicle_pid_values -> kp;
	ki = vehicle_pid_values -> ki;
	kd = vehicle_pid_values -> kd;
	pthread_mutex_unlock(&pid_values_mutex);

	sprintf(msg, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f,%f,%f",
		position_lat,
		position_lon,
		thrust_1,
		thrust_2,
		thrust_3,
		thrust_4,
		orientation_pitch,
		orientation_roll,
		orientation_yaw,
		desired_orientation_pitch,
		desired_orientation_roll,
		desired_orientation_yaw,
		mode,
		kp,
		ki,
		kd);

	return msg;	
}
