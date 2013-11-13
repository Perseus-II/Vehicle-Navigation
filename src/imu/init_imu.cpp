#include "../../lib/perseus.h"
#include "../../lib/imu.h"
#include "../../lib/quaternion.h"
#include "../extern.h"

#define MAX 256

int imu_fd = -1; /* global arduino communication fd */

XimuReceiver ximu;

using namespace std;

void connect_to_imu();

void *init_imu(void *data) {
	char buf[1];
	int n;
	/* not connected to imu, attempt to reconnect */
	if(imu_fd < 0) connect_to_imu();

	/* open up a serial connection to the x-imu */ 
	while(1) {
		if((n=read(imu_fd, buf, 1)) > 0) {
			ximu.processNewChar(buf[0]);
		}
		
		if(ximu.isBattAndThermGetReady()) {
		}
		if(ximu.isInertialAndMagGetReady()) {
		}
		if(ximu.isQuaternionGetReady()) {
			QuaternionStruct quaternionStruct = ximu.getQuaternion();
			Quaternion quaternion = Quaternion(quaternionStruct.w, quaternionStruct.x, quaternionStruct.y, quaternionStruct.z);
			EulerAnglesStruct eulerAnglesStruct = quaternion.getEulerAngles();
			//cout << YELLOW << "[DEBUG] Updating IMU Values:" << endl;
			//cout << "\tPitch: " << eulerAnglesStruct.pitch << endl;
			//cout << "\tRoll: " << eulerAnglesStruct.roll << endl;
			//cout << "\tYaw: " << eulerAnglesStruct.yaw << RESET << endl;
			/* update global orientation struct */
			pthread_mutex_lock(&orientation_mutex);	
			vehicle_orientation -> pitch = eulerAnglesStruct.pitch;
			vehicle_orientation -> roll = eulerAnglesStruct.roll;
			vehicle_orientation -> yaw = eulerAnglesStruct.yaw;
			pthread_mutex_unlock(&orientation_mutex);	

		}
		if(write(imu_fd, "",1) < 0) {
			printf(RED "[ERROR] Lost connection to IMU... Reconnecting...\n" RESET, buf);
			close(imu_fd);
			connect_to_imu();
		}
	
	}
}

void connect_to_imu() {
	char buf[MAX];

	strcpy(buf, "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AM01L7QD-if00-port0");

	imu_fd = open(buf, O_RDWR | O_NOCTTY | O_SYNC);
	if(imu_fd < 0) {
		printf(RED "[ERROR] Failed to open /dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AM01L7QD-if00-port0\n" RESET);
	}
	set_interface_attribs(imu_fd, B115200, 0);
	set_blocking(imu_fd, 1);
	printf(YELLOW "[DEBUG] IMU: Connected to X-imu" RESET "\n");
}

