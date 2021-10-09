/**(C) Copyright 2014 Rafael Marsolla - rafamarsolla@gmail.com
 *
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

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
