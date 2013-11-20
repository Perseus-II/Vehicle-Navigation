/**
 * This file contains the TCP port designations for the various modules in use
 * Please do not modify unless absolutely necessary. Other applications/modules may be 
 * dependent on these specific port designations.
 */
#ifndef __PORT_DESIGNATIONS_H__
#define __PORT_DESIGNATIONS_H__

#ifndef MISSION_CONTROL_PORT
#define MISSION_CONTROL_PORT 9010
#endif /* MISSION_CONTROL_PORt */

#ifndef DIAGNOSTICS_PORT
#define DIAGNOSTICS_PORT 9011
#endif /* DIAGNOSTICS_PORT */

#ifndef	BEAGLEBONE_SURFACE_IP
#define	BEAGLEBONE_SURFACE_IP "192.168.10.91"
#endif /* BEAGLEBONE_SURFACE_IP */

#ifndef	BEAGLEBONE_SURFACE_PORT
#define	BEAGLEBONE_SURFACE_PORT 1477
#endif /* BEAGLEBONE_SURFACE_IP */


#endif /* __PORT_DESIGNATIONS_H__ */
