#ifndef c37118header_h
#define c37118header_h

#include "c37118.h"

/**
* C37.118-2011 HEADER FRAME CLASS
* Methods, Get/Set for each field to send and get Header Information
* Usualy is a developer data, like SerialNumber, Version, etc
* Note:DATA is a dynamic size, defined by user application
*       Size(DATA) = Size(FRAMESIZE) - 16 bytes
*/
class HEADER_Frame : public C37118{
	
public: 
	HEADER_Frame(string info);
	
	/**
	* SET Methods
	*/
	void DATA_set(string value);

	/**
	* GET Methods
	*/
	string DATA_get();
	
	/**
	* PACK/UNPACK Methods
	*/
	
	unsigned short pack(unsigned char **buff);
	void unpack(unsigned char *buffer);
	
	/**
	* DATA STORE
	*/	
	protected:
	string info;
};
#endif
