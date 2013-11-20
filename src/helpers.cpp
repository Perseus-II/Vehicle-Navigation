#include "../lib/perseus.h"
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




using namespace std;

int strindex(char *haystack, char needle) {
	if(haystack == NULL) return -1;
	int i;
	
	for(i=0; i < strlen(haystack); i++) {
		if(haystack[i] == needle)
			return i;
	}
}

void strip_newline(char *str) {
	str[strindex(str, '\n')] = '\0';
}

void send_response(char* (*func)(), int sockfd) {
	/* This function handles sending a message
	 * back to the client using the response from
	 * the passed function pointer */
	char *res;
	res = func();
	res[strlen(res)] = '\n';
	if(write(sockfd, res, strlen(res)) < 0)
	       perror("Failed to write response to client");	
	free(res);
}

int set_interface_attribs(int fd, int speed, int parity) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		printf(RED "[ERROR] error %d from tcgetattr\n" RESET, errno);
		return -1;
	}

	cfsetospeed (&tty, speed);
	cfsetispeed (&tty, speed);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tty.c_iflag &= ~IGNBRK;         // ignore break signal
	tty.c_lflag = 0;                // no signaling chars, no echo,
	// no canonical processing
	tty.c_oflag = 0;                // no remapping, no delays
	tty.c_cc[VMIN]  = 0;            // read doesn't block
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
	// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		printf(RED "[ERROR] error %d from tcsetattr" RESET, errno);
		return -1;
	}
	return 0;
}

void set_blocking(int fd, int should_block) {
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		printf(RED "[ERROR] error %d from tggetattr\n" RESET, errno);
		return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
		printf(RED "[ERROR] error %d setting term attributes\n" RESET, errno);
}

float arr_sum(float *arr, int len) {
	float sum;
	int i;
	for(i=0; i<len; i++) {
		sum += arr[i];
	}
	return sum;
}
	
