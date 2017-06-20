#ifndef __PATHMACROS_H__
#define __PATHMACROS_H__

// LED via SODIMM pin 55 (Linux GPIO 14)     
#ifdef __ARM_EABI__    //Path when executing on target
	#define PATH_LED_VALUE		"/sys/class/gpio/gpio2/value"
	#define PATH_LED_DIRECTION	"/sys/class/gpio/gpio2/direction"
#else                  //Path when debugging on host
	#define PATH_LED_VALUE		"/tmp/gpio58/value"
	#define PATH_LED_DIRECTION	"/tmp/gpio58/direction"
	
#endif
#endif
