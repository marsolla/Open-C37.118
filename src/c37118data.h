#ifndef c37118data_h
#define c37118data_h

#include "c37118.h"
#include "c37118configuration.h"
#include <stdio.h>
#include <stdlib.h>
#include <complex>
#include <iostream>

/**
* C37.118-2011 DATA FRAME CLASS
* Methods, Get/Set for each field to send and get Phasor Data frame
* Note: DATA FRAME is a dynamic size, defined by a numbers of PMU
*/
class DATA_Frame : public C37118{
	public:	
	DATA_Frame(CONFIG_Frame *cfg);	
		
	/**
	* SET Methods
	*/


	/**
	* GET Methods
	*/
	
	
	/**
	* PACK/UNPACK Methods
	*/
	unsigned short pack(unsigned char **buff);
	void unpack(unsigned char *buffer);
	
	/**
	* DATA STORE
	*/	
	CONFIG_Frame *associate_current_config;
	
};
#endif
