#include "../../lib/vehicle_control.h"
#include "../../lib/perseus.h"
#include "../extern.h"
#include <sys/time.h>
#include <math.h>
#include <ctime>

#define ABS(x) (((x)<0)?(-x):(x))
#define PI 3.14159265
#define NUM_ERRORS 20

float arr_sum(float*,int);

void *init_autonomous_depth(void *data) {
	int i = 0;
	struct timeval prev_time, cur_time;
	gettimeofday(&prev_time, NULL);

	float depth_pid;
	float depth_prev_error=0;
	/* store error for the last 40 samples (2 second) */
	float depth_errors[NUM_ERRORS];
	long dt, seconds, useconds;    

	int mode;

	float kp, ki, kd;
	float error;

	float s_depth, sd_depth;

	while(1) {
		/* we're not in autonomous mode, don't change anything */
		pthread_mutex_lock(&mode_mutex);
		mode = vehicle_mode;
		pthread_mutex_unlock(&mode_mutex);
		if(mode != MODE_AUTON_DEPTH) continue;

		/* get the current PID values */
		pthread_mutex_lock(&pid_values_mutex);
		kp = vehicle_pid_values -> kp;
		ki = vehicle_pid_values -> ki;
		kd = vehicle_pid_values -> kd;
		pthread_mutex_unlock(&pid_values_mutex);

		i = (i+1)%NUM_ERRORS; /* cyclic buffer */

		gettimeofday(&cur_time, NULL);
		seconds  = cur_time.tv_sec  - prev_time.tv_sec;
		useconds = cur_time.tv_usec - prev_time.tv_usec;
		/* compute the delta-time since last PID interation */
		dt = ((seconds) * 1000 + useconds/1000.0) + 0.5;
		prev_time = cur_time;

	
		/* perform a PID loop on state -> desired_state */	
		pthread_mutex_lock(&dst_mutex);
		s_depth = vehicle_dst  -> depth_ft;
		pthread_mutex_unlock(&dst_mutex);

		/* get the desired state vector */
		pthread_mutex_lock(&desired_orientation_mutex);
		sd_depth = desired_vehicle_orientation -> depth; 
		pthread_mutex_unlock(&desired_orientation_mutex);
		

		error = sd_depth - s_depth;
		
		depth_errors[i] = error;
		
		depth_pid = (kp*depth_errors[i]) +
			  (ki*arr_sum(depth_errors,NUM_ERRORS)) +
			  (kd*((depth_errors[i]-depth_prev_error)/dt));

		

		printf("[%d]\t%f -> %f\tdepthpid=%f\terror=%f\n", vehicle_mode, s_depth, sd_depth, depth_pid,error);

		if(depth_pid > 65) depth_pid = 65;
		if(depth_pid < -65) depth_pid = -65;

		pthread_mutex_lock(&thrust_mutex);
		vehicle_thrust -> heave_b = depth_pid; 
		pthread_mutex_unlock(&thrust_mutex);

		depth_prev_error = depth_errors[i];
		usleep(50000); /* sleep for 50ms */
	}
	
}

