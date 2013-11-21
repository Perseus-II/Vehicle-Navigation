#ifndef __MISSION_CONTROL_H__
#define __MISSION_CONTROL_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

/* actions */
char *set_thrust(float, float, float, float); 
char *set_orientation(float, float, float);
char *set_vehicle_mode(int);
char *set_pid_values(float,float,float);
char *set_current_orientation();
char *set_desired_depth(float);
char *set_la(int, int);

void *mission_control_handler(void*);
void *init_mission_control(void*);

#endif /* __MISSION_CONTROL_H__ */
