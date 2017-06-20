/// @file		gpio_output.c
/// @copyright		Copyright (c) 2012 Toradex AG
/// @author		Satyan Raj	
/// @date		18/Apr/2013
/// @brief		Program blink LED on Colibri Evaluation Board (V2.1 or V3.1) or IRIS Borad
///			Program sill configure SODIMM 55 (GPIO 14) as output pin.
///			Program will blink the LED at 
///				Frequency : 1Hz, DutyCyclc : 50%							
///			PRESS 'q' on keybaord to exit.		
/// @target		Colibri T20 + Linux for Tegra
/// @caveats		NIL

#include <fcntl.h>                      // for open(), close(), lseek()
#include <sys/stat.h>                   // for open()
#include <stdio.h>			// standard I/O header file
#include <sys/types.h>                  // for open()
#include <unistd.h>                     // for write(), open(), read(), usleep()
#include "getkey.h"			// read keyboard input
#include "pathmacros.h"			// path for GPIO files


int main(int argc, char **argv)
{
        char key = -1;               // default : No key pressed
        unsigned char led = '0';     // low(0) for Output, ASCII(48d), DEC(0d), HEX(0x30)
        int retVal;                  // for debugging purpose
        int fdLedValue;              // file descriptor : Holds value for GPIO value (output)
        int fdLedDirection;          // file descriptor : Holds value for GPIO direction
        char outputString[] = "out"; // to set GPIO direction as OUTPUT

//  Configure GPIO direction as Output
        fdLedDirection = open(PATH_LED_DIRECTION, O_RDWR);     // open file PATH_LED_DIRECTION in read/write mode
                                                               // parameter: open(filepath, read-write permissions),
                                                               // returns: file descriptor or -1 if an error occurred.
        retVal = write(fdLedDirection, &outputString[0], 4);   // write into file to configure GPIO as output
                                                               // parameter: write(file_Descriptor, data_buffer, no_of_bytes),
                                                               // returns: No. of bytes written or -1 if an error occurred.
        retVal = close(fdLedDirection);                        // close file with file descriptor
                                                               // parameter: open(filepath, read-write permissions),
                                                               // returns: 0 on success or -1 if an error occurred.

        fdLedValue = open(PATH_LED_VALUE, O_RDWR);             // Open PATH_LED_VALUE file beforehand.
                                                               // parameter: open(filepath, read-write permissions),
                                                               // returns: file descriptor or -1 if an error occurred.

//  Blink LED, by writing the I/O state value into the file: PATH_LED_VALUE
        do{
            if(led == '1')
                      led = '0';                               // Change led value, ASCII: '1' ----> '0'
            else
                      led = '1';                               // Change led value, ASCII: '0' ----> '1'

                retVal = write(fdLedValue, &led, 1);           // write into file to update LED value - Low(0) or High (1)
                key = GetKey();                                // get user input from keyboard
                usleep(500);                                // create a delay of 500 milliseconds
        }while(key != 'q');                                    // exit if keyboard input is 'q'

        retVal = close(fdLedValue);                            // close file with file descriptor

    printf("\nExiting.... \n");

    return(0);
}


