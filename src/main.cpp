#include "../lib/perseus.h"
#include "../lib/diagnostics.h"
#include "../lib/mission_control.h"
#include "../lib/thruster_control.h"
#include "../lib/imu.h"
#include "../lib/vehicle_control.h"
#include "../lib/dst.h"
#include "../lib/linear_actuator.h"

pthread_mutex_t position_mutex;
position_t *vehicle_position;

pthread_mutex_t thrust_mutex;
vehicle_thrust_t *vehicle_thrust;

pthread_mutex_t orientation_mutex;
vehicle_orientation_t *vehicle_orientation;

pthread_mutex_t desired_orientation_mutex;
vehicle_orientation_t *desired_vehicle_orientation;

pthread_mutex_t mode_mutex;

pthread_mutex_t pid_values_mutex;
pid_values_t *vehicle_pid_values;

pthread_mutex_t initial_orientation_mutex;
vehicle_orientation_t *vehicle_initial_orientation;

pthread_mutex_t dst_mutex;
vehicle_dst_t *vehicle_dst;

int vehicle_mode;

pthread_mutex_t linear_actuator_mutex;
linear_actuator_t *linear_actuator_state;

int main(int args, char **argv) {
	pthread_t diagnostics;
	pthread_t mission_control;
	pthread_t vehicle_control;	/* handles navigation (point A -> point B)
       					   converts to low-level thruster commands */

	pthread_t thruster_control;	/* communicates with thruster arduino (A0) */
	pthread_t gps_fix;		/* communicates with beaglebone at surface */
	pthread_t imu;
	pthread_t dst;
	pthread_t autonomous_depth;
	pthread_t linear_actuator_thread;
	
	/* allocate memory for shared variables */
	vehicle_position = (position_t*)malloc(sizeof(position_t));
	vehicle_thrust = (vehicle_thrust_t*)malloc(sizeof(vehicle_thrust_t));
	vehicle_orientation = (vehicle_orientation_t*)malloc(sizeof(vehicle_orientation_t));
	desired_vehicle_orientation = (vehicle_orientation_t*)malloc(sizeof(vehicle_orientation_t));
	vehicle_pid_values = (pid_values_t*)malloc(sizeof(pid_values_t));
	vehicle_initial_orientation = (vehicle_orientation_t*)malloc(sizeof(vehicle_orientation_t));
	vehicle_dst = (vehicle_dst_t*)malloc(sizeof(vehicle_dst_t));
	linear_actuator_state = (linear_actuator_t*)malloc(sizeof(linear_actuator_t));

	/* initialize mutexes */
	pthread_mutex_init(&position_mutex, NULL);
	pthread_mutex_init(&thrust_mutex, NULL);
	pthread_mutex_init(&orientation_mutex, NULL);
	pthread_mutex_init(&desired_orientation_mutex, NULL);
	pthread_mutex_init(&mode_mutex, NULL);
	pthread_mutex_init(&pid_values_mutex, NULL);
	pthread_mutex_init(&initial_orientation_mutex, NULL);
	pthread_mutex_init(&dst_mutex, NULL);
	pthread_mutex_init(&linear_actuator_mutex, NULL);

	/* set default vehicle mode */
	/* if we start the control thread before setting the mode, 
	   the vehicle may take off in a random direction */
	pthread_mutex_lock(&mode_mutex);
	vehicle_mode = MODE_MANUAL;
	pthread_mutex_unlock(&mode_mutex);

	/* set up an initial vehicle position (1,1,1) */
	pthread_mutex_lock(&position_mutex);
	vehicle_position -> x = 0.0;
	vehicle_position -> y = 0.0;
	vehicle_position -> z = 0.0;
	pthread_mutex_unlock(&position_mutex);

	/* set up initial desired vehicle orientation */
	pthread_mutex_lock(&desired_orientation_mutex);
	desired_vehicle_orientation -> pitch	= 	0.0;
	desired_vehicle_orientation -> roll 	= 	0.0;
	desired_vehicle_orientation -> yaw 	= 	0.0;
	desired_vehicle_orientation -> depth 	= 	0.0;
	pthread_mutex_unlock(&desired_orientation_mutex);

	/* set up initial vehicle orientation */
	pthread_mutex_lock(&orientation_mutex);
	vehicle_orientation -> pitch	= 	0.0;
	vehicle_orientation -> roll 	= 	0.0;
	vehicle_orientation -> yaw 	= 	0.0;
	pthread_mutex_unlock(&orientation_mutex);

	/* set up initial vehicle thrust and vectoring */
	pthread_mutex_lock(&thrust_mutex);
	vehicle_thrust -> surge_port		=	0.0;
	vehicle_thrust -> surge_starboard 	= 	0.0;
	vehicle_thrust -> heave_a 		= 	0.0;
	vehicle_thrust -> heave_b		= 	0.0;
	pthread_mutex_unlock(&thrust_mutex);

	/* set up initial pid values */
	pthread_mutex_lock(&pid_values_mutex);
	vehicle_pid_values -> kp = 0.18;
	vehicle_pid_values -> ki = 0.01;
	vehicle_pid_values -> kd = 0.04;
	pthread_mutex_unlock(&pid_values_mutex);

	pthread_mutex_lock(&initial_orientation_mutex);
	vehicle_initial_orientation -> pitch = 0;
	vehicle_initial_orientation -> roll = 0;
	vehicle_initial_orientation -> yaw = 0;
	pthread_mutex_unlock(&initial_orientation_mutex);

	pthread_mutex_lock(&dst_mutex);
	vehicle_dst -> temperature = 0.0;
	vehicle_dst -> speed_knts = 0.0;
	vehicle_dst -> depth_m = 0.0;
	vehicle_dst -> depth_ft = 0.0;
	pthread_mutex_unlock(&dst_mutex);

	pthread_mutex_lock(&linear_actuator_mutex);
	linear_actuator_state -> leds_on = FALSE;
	linear_actuator_state -> laser_on = FALSE;
	linear_actuator_state -> la_on = 0;
	pthread_mutex_unlock(&linear_actuator_mutex);

	void *status;
	int i;
	int rc;	
	rc = 0;

	pthread_create(&diagnostics, NULL, init_diagnostics, NULL);	
	pthread_create(&mission_control, NULL, init_mission_control, NULL);
	pthread_create(&thruster_control, NULL, init_thruster_control, NULL);
	pthread_create(&vehicle_control, NULL, init_vehicle_control, NULL);
	pthread_create(&gps_fix, NULL, init_gps_fix, NULL);
	pthread_create(&imu, NULL, init_imu, NULL);
	pthread_create(&dst, NULL, init_dst, NULL);
	pthread_create(&autonomous_depth, NULL, init_autonomous_depth, NULL);
	pthread_create(&linear_actuator_thread, NULL, init_la, NULL);

	/* Initialize camera feeds */
	init_camera_feed(0,9997);
	init_camera_feed(1,9998);
	init_camera_feed(2,9999);
	
	/* wait for all threads to terminate */
	pthread_join(diagnostics, NULL);
	pthread_join(mission_control, NULL);
	pthread_join(thruster_control, NULL);
	pthread_join(vehicle_control, NULL);
	pthread_join(gps_fix, NULL);
	pthread_join(imu, NULL);
	pthread_join(dst, NULL);
	pthread_join(autonomous_depth, NULL);
	pthread_join(linear_actuator_thread, NULL);
}

