#ifndef __DIAGNOSTICS_H__
#define __DIAGNOSTICS_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>

/* actions */
char* water_temp();
char *current_position();
char *battery_status();
char *get_vehicle_thrust();
char *get_vehicle_orientation();
char *get_vehicle_info();


void *diagnostics_handler(void*);
void *init_diagnostics(void *);


#endif /* __DIAGNOSTICS_H_ */
