/// @file		GetKey.c
/// @copyright		Copyright (c) 2012 Toradex AG
/// @author		Satyan Raj	
/// @date		15/Mar/2013
/// @brief		Program to find the key pressed by user on keyboard
///			
/// @target		Colibri T20 + Linux for Tegra
/// @caveats		NIL

#include <stdio.h>			// standard I/O header file
#include <string.h>
#include <termios.h>
#include "getkey.h"


int GetKey(void)
{	
	struct termios orig_term_attr; 			// holds original state of terminal
	struct termios new_term_attr;			// holds new state of terminal
	int character;				

//******************************************************************************
//	Set terminal in raw mode, to access keyboard input
//******************************************************************************
	tcgetattr(fileno(stdin), &orig_term_attr);	// fileno retruns the file descriptor number of stdin

	memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));// copy date from orig_term_attr to new_term_attr	
	new_term_attr.c_lflag &= ~ICANON;		// Non-Canonical input processing --> no ENTER required 
	new_term_attr.c_cc[VTIME] = 0;			// Wait for 0*0.01 sec input, in Non-Canonical state
	new_term_attr.c_cc[VMIN] = 0;			// VMIN is min number of bytes that must be available in input queue, in Non-Canonical state

	tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);// set new attributes to stdin

	character=fgetc(stdin);				// read for stdin and return EOF(-1), if no character is available 

//******************************************************************************
//	restore terminal to its original state
//******************************************************************************
	tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);// restore original terminla attributes, TCSANOW: apply changes now	
	return(character);				
}

