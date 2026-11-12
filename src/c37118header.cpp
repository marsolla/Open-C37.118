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

#include "c37118header.h"


/**
* PUBLIC Methods from CLASS HEADER FRAME, Get/Set for each field
* Note: DATA is a dynamic size, defined by user application
*       Size(DATA) = Size(FRAMESIZE) - 16 bytes
*/

/**
* Method Constructor
* Init the 2 first byte with Header Frame ID
* Add Header 0xAA|0x11
* Set the default FRAMESIZE with 16 bytes.
* If use DATA field, please update the FRAMESIZE too.
*/
HEADER_Frame::HEADER_Frame(string info){
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_HDR);
	this->FRAMESIZE = 16;
	this->info = info;
}

/**
*  Set DATA Field from Header Frame
*/
void HEADER_Frame::DATA_set(string value){
	this->info = value;	
}

/**
*  Get DATA Field from Header Frame
*/
string HEADER_Frame::DATA_get(){
	return this->info;
}


/**
* Method unpack_cmd_frame
* Received a frame buffer, parse all fields until FRAMESIZE has been reached
* Values stored in this class
*/
void HEADER_Frame::unpack(unsigned char *buffer, unsigned int buffer_len){
     	unsigned char *buffer2;
     	unsigned short size_data;
	// Reject frames that do not carry the 14-byte header, and reject a FRAMESIZE that exceeds the bytes
	// actually received before it sizes the DATA copy and locates the trailing CHK.
	if (buffer_len < 16) return;
		this->SYNC_set(ntohs(*((unsigned short*)(buffer))));
		this->FRAMESIZE_set(ntohs(*((unsigned short*)(buffer+2))));
		this->IDCODE_set(ntohs(*((unsigned short*)(buffer+4))));
		this->SOC_set(ntohl(*((unsigned int*)(buffer+6))));
		this->FRACSEC_set(ntohl(*((unsigned int*)(buffer+10))));
		unsigned int framesize = this->FRAMESIZE_get();
		if (framesize < 16 || framesize > buffer_len) return;
		buffer2 = buffer + 14; 
		
		size_data = this->FRAMESIZE_get()-16;
		char * cstr = new char [size_data + 1];
		for(int ptr =0 ; ptr< size_data;ptr++){
			cstr[ptr] = buffer2[ptr];
		}
		cstr[size_data] = '\0';           // NUL-terminate; the buffer was previously unterminated
		this->DATA_set(string(cstr, size_data));
		delete [] cstr;                   // was leaked on every call
		
		this->CHK_set(ntohs(*((unsigned short *)(buffer+(this->FRAMESIZE_get()-2)))));
}

/**
* Method pack_cmd_buffer
* Mount a new command frame based on values store in the the class,
* special short and long pointers are needs to populate the buffer.
*/
unsigned short HEADER_Frame::pack(unsigned char **buff){
	unsigned char *aux_buff;
	unsigned short *shptr;
	unsigned long *lptr;
	string str = this->DATA_get();
	
	unsigned short size = 16 + str.size();
	
	// set frame size
	this->FRAMESIZE_set(size);
	
	//buff size reserved
	*buff = (unsigned char *) malloc (this->FRAMESIZE_get()*sizeof(char));
	//copy buff memory address
	aux_buff = *buff;
	//create a short and long pointers, and increment by byte_size(2,4...)
	shptr=(unsigned short *) (aux_buff);
	*shptr = htons(this->SYNC_get()); aux_buff +=2;
	shptr=(unsigned short *) (aux_buff);
	*shptr = htons(this->FRAMESIZE_get()); aux_buff +=2;
	shptr=(unsigned short *) (aux_buff);
	*shptr = htons(this->IDCODE_get()); aux_buff +=2;
	lptr=(unsigned long *) (aux_buff);
	*lptr = htonl(this->SOC_get()); aux_buff +=4;
	lptr=(unsigned long *) (aux_buff);
	*lptr = htonl(this->FRACSEC_get()); aux_buff +=4;
	char * cstr = new char [str.size()];
	//Get name string and convert to char string
	strcpy (cstr, str.c_str());
	for(int ptr =0 ; ptr< str.size();ptr++){
		aux_buff[ptr]=cstr[ptr];
	}
	aux_buff += str.size();
	
	// Compute CRC from current frame
	unsigned short crc_aux = this->Calc_CRC(*buff,this->FRAMESIZE_get()-2);
	this->CHK_set(crc_aux);
	
	aux_buff = *buff + (this->FRAMESIZE_get()-2);
	shptr=(unsigned short *) (aux_buff);
	*shptr = htons(this->CHK_get());
	return (this->FRAMESIZE_get());
}
