#ifndef __GETKEY_H__
#define __GETKEY_H__


/// Read keyboard and doesn't wait for "ENTER"
/// @retval	-1 	No keys pressed
/// @retval	>0 	ASCII code recieved

int GetKey(void);

#endif

