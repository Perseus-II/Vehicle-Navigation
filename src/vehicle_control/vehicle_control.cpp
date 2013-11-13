#include "../../lib/perseus.h"
#include "../../lib/vehicle_control.h"
#include "../extern.h"
#include <sys/time.h>
#include <math.h>
#include <ctime>

#define ABS(x) (((x)<0)?(-x):(x))
#define PI 3.14159265
#define NUM_ERRORS 20

float arr_sum(float*,int);

void *init_vehicle_control(void *data) {
	printf(YELLOW "[DEBUG] Init vehicle control thread\n" RESET);

	float s_pitch, s_roll, s_yaw;
	float sd_pitch, sd_roll, sd_yaw;
	int i = 0;
	struct timeval prev_time, cur_time;
	gettimeofday(&prev_time, NULL);

	float yaw_pid;
	float yaw_prev_error=0;
	/* store error for the last 40 samples (2 second) */
	float yaw_errors[NUM_ERRORS];
	long dt, seconds, useconds;    

	int mode;

	double cos_td_m_cos_ta;

	float kp, ki, kd;
	float error;

	float sppid,sspid;

	while(1) {
		/* we're not in autonomous mode, don't change anything */
		pthread_mutex_lock(&mode_mutex);
		mode = vehicle_mode;
		pthread_mutex_unlock(&mode_mutex);
		if(mode != MODE_AUTON) continue;

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
		pthread_mutex_lock(&orientation_mutex);
		s_pitch = vehicle_orientation -> pitch;
		s_roll = vehicle_orientation -> roll;
		s_yaw = vehicle_orientation -> yaw;
		pthread_mutex_unlock(&orientation_mutex);

		/* get the desired state vector */
		pthread_mutex_lock(&initial_orientation_mutex);
		sd_pitch = vehicle_initial_orientation -> pitch;	
		sd_roll = vehicle_initial_orientation -> roll;	
		sd_yaw = vehicle_initial_orientation -> yaw;	
		pthread_mutex_unlock(&initial_orientation_mutex);
		

		error = abs(abs(sd_yaw*1.0) - abs(s_yaw*1.0));
		
		yaw_errors[i] = error;
		
		yaw_pid = (kp*yaw_errors[i]) +
			  (ki*arr_sum(yaw_errors,NUM_ERRORS)) +
			  (kd*((yaw_errors[i]-yaw_prev_error)/dt));

		
		sspid = 0;
		sppid = 0;
		
		cos_td_m_cos_ta = cos((sd_yaw*PI)/180.0) - cos((s_yaw*PI)/180.0);
		//printf("cos_sdyaw = %f \t cos_syaw = %f\n", cos((sd_yaw*180)/PI), cos((s_yaw*180)/PI));

		if((s_yaw > 0 && cos_td_m_cos_ta > 0) || (s_yaw < 0 && cos_td_m_cos_ta < 0)) {
			sspid = -1*yaw_pid;
			//puts("turn clockwise");
		}
		if((s_yaw < 0 && cos_td_m_cos_ta > 0) || (s_yaw > 0 && cos_td_m_cos_ta < 0)) {
			sppid = -1*yaw_pid;
			//puts("turn counterclockwise");
		}

		printf("cos = %f\n", cos_td_m_cos_ta);

		sspid += 30;
		sppid += 30;
	
		sspid = (sspid > 75) ? 75 : sspid;
		sppid = (sppid > 75) ? 75 : sppid;

		printf("[%d]\t%f -> %f |\t(%f,%f)\tyawpid=%f\terror=%f\n", vehicle_mode, s_yaw, sd_yaw, sppid, sspid, yaw_pid,error);

		pthread_mutex_lock(&thrust_mutex);
		vehicle_thrust -> surge_starboard = sspid; 
		vehicle_thrust -> surge_port = sppid; 
		pthread_mutex_unlock(&thrust_mutex);
		
		yaw_prev_error = yaw_errors[i];
		usleep(50000); /* sleep for 50ms */
	}
	
}

float arr_sum(float *arr, int len) {
	float sum;
	int i;
	for(i=0; i<len; i++) {
		sum += arr[i];
	}
	return sum;
}

