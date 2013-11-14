#ifndef __DST_H__
#define __DST_H__

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

void *init_dst(void*);

#endif /* __DST_H__ */
