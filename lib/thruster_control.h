#ifndef __THRUSTER_CONTROL_H__
#define __THRUSTER_CONTROL_H__

/* actions */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>


void *init_thruster_control(void*);

#endif /* __THRUSTER_CONTROL_H__ */
