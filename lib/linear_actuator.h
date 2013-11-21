#ifndef __LINEAR_ACTUATOR_H__
#define __LINEAR_ACTUATOR_H__

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


void *init_la(void*);

#endif

