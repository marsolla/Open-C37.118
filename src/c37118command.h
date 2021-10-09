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
