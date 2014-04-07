#ifndef c37118command_h
#define c37118command_h

#include "c37118.h"

/**
* C37.118-2011 COMMAND FRAME CLASS
* Methods, Get/Set for each field to interpret the command
* Note: EXTRAFRAME is a dynamic size, defined by user application
*       Size(EXTRAFRAME) = Size(FRAMESIZE) - 18 bytes
*/
class CMD_Frame : public C37118{
	
	public: 
	CMD_Frame();
	
	/**
	* SET Methods
	*/
	void CMD_set(unsigned short value);
	void EXTRAFRAME_set(unsigned char *value);

	/**
	* GET Methods
	*/
	unsigned short CMD_get();
	unsigned char *EXTRAFRAME_get();
	
	/**
	* PACK/UNPACK Methods
	*/
	unsigned short pack(unsigned char **buffer);
	void unpack(unsigned char *buffer);
	
	/**
	* DATA STORE
	*/	
	protected:
	unsigned short CMD;
	unsigned char *EXTRAFRAME;	
};
#endif
