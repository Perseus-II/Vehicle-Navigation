#ifndef __PERSEUS_H__
#define __PERSEUS_H__

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef int t_boolean;

#ifndef TRUE
#define TRUE ((int)1)
#endif

#ifndef FALSE
#define FALSE ((int)0)
#endif


/* helpers */
void strip_newline(char*);
int strindex(char*, char);
void send_response(char* (*func)(), int);
int set_interface_attribs(int,int,int);
void set_blocking(int,int);

void init_camera_feed(int,int);

void *init_gps_fix(void*);

#define MODE_DISABLED 0
#define MODE_MANUAL 1
#define MODE_AUTON 2

typedef struct pid_values_s {
	float kp;
	float ki;
	float kd;
} pid_values_t;

typedef struct vehicle_orientation_s {
	float pitch;
	float roll;
	float yaw;
} vehicle_orientation_t;

typedef struct position_s {
	float x;
	float y;
	float z;
	double lat;
	double lon;
	int sv_in_view;
	int sv_in_use;
	float horizontal_dil;
} position_t;

typedef struct vehicle_thrust_s {
	float surge_port;
	float surge_starboard;
	float heave_a;
	float heave_b;
} vehicle_thrust_t;

typedef struct vehicle_dst_s {
	float temperature;
	float speed_knts;
	float depth_m;
	float depth_ft; 
} vehicle_dst_t;


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#endif
