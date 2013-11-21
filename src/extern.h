#ifndef __GLOBALS__
#define __GLOBALS__

extern pthread_mutex_t		position_mutex;
extern position_t 		*vehicle_position;

extern pthread_mutex_t		thrust_mutex;
extern vehicle_thrust_t		*vehicle_thrust;

extern pthread_mutex_t		orientation_mutex;
extern vehicle_orientation_t	*vehicle_orientation;

extern pthread_mutex_t		desired_orientation_mutex;
extern vehicle_orientation_t	*desired_vehicle_orientation;

extern int 			vehicle_mode;
extern pthread_mutex_t		mode_mutex;

extern pthread_mutex_t		pid_values_mutex;
extern pid_values_t		*vehicle_pid_values;

extern pthread_mutex_t 		initial_orientation_mutex;
extern vehicle_orientation_t	*vehicle_initial_orientation;

extern pthread_mutex_t		dst_mutex;
extern vehicle_dst_t		*vehicle_dst;

extern linear_actuator_t	*linear_actuator_state;
extern pthread_mutex_t		linear_actuator_mutex;

#endif /* __GLOBALS__ */
