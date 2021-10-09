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
