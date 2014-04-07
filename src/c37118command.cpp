
#include "c37118command.h"

/**
* PUBLIC Methods from CLASS COMMAND FRAME, Get/Set for each field
* Note: EXTRAFRAME is a dynamic size, defined by user application
*       Size(EXTRAFRAME) = Size(FRAMESIZE) - 18 bytes
*/

/**
* Method Constructor
* Init the 2 first byte with Command Frame ID
* Add Header 0xAA|0x41
* Set the default FRAMESIZE with 18 bytes.
* If use EXTRAFRAME field, please update the FRAMESIZE too.
*/
// Constructor - Add CMD Frame Header 0xAA|0x41
CMD_Frame::CMD_Frame(){
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_CMD);
	this->FRAMESIZE = 18;
}

/**
*  Set EXTRAFRAME Field from Command Frame
*/
void CMD_Frame::EXTRAFRAME_set(unsigned char *value){
	this->EXTRAFRAME = value;	
}
/**
*  Set CMD Field from Command Frame
*/
void CMD_Frame::CMD_set(unsigned short value){
	this->CMD = value;	
}

/**
*  Get CMD Field from Command Frame
*/
unsigned short CMD_Frame::CMD_get(){
	return this->CMD;
}
/**
*  Get EXTRAFRAME Field from Command Frame
*/
unsigned char *CMD_Frame::EXTRAFRAME_get(){
	return this->EXTRAFRAME;
}

/**
* Method unpack_cmd_frame
* Received a frame buffer, parse all fields until FRAMESIZE has been reached
* Values stored in this class
*/
void CMD_Frame::unpack(unsigned char *buffer){
     	unsigned char *buffer2;
		this->SYNC_set(ntohs(*((unsigned short*)(buffer))));
		this->FRAMESIZE_set(ntohs(*((unsigned short*)(buffer+2))));
		this->IDCODE_set(ntohs(*((unsigned short*)(buffer+4))));
		this->SOC_set(ntohl(*((unsigned long*)(buffer+6))));
		this->FRACSEC_set(ntohl(*((unsigned long*)(buffer+10))));
		this->CMD_set(ntohs(*((unsigned short*)(buffer+14))));
		// EXTRAFRAME needs a special treatment, user defined data
		// unsigned char array to store the data
		int ptr = 0;
		this->EXTRAFRAME = (unsigned char *)malloc (sizeof(char)*(this->FRAMESIZE_get()-18));
		for (ptr; ptr < this->FRAMESIZE_get()-18 ; ptr++){
			this->EXTRAFRAME[ptr] =  buffer[ptr+16] ;
		}
		this->CHK_set(ntohs(*((unsigned short *)(buffer+(this->FRAMESIZE_get()-2)))));
		//printf("Packet Received: %x,%x,%x,%x,%x,%x,%x\n",this->FRAMESIZE_get(),this->IDCODE_get(),this->SOC_get(),this->FRACSEC_get(),this->CMD_get(),this->EXTRAFRAME_get(),this->CHK_get());
}

/**
* Method pack_cmd_buffer
* Mount a new command frame based on values store in the the class,
* special short and long pointers are needs to populate the buffer.
*/
unsigned short CMD_Frame::pack(unsigned char **buff){
	unsigned char *aux_buff;
	unsigned short *shptr;
	unsigned long *lptr;
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
	shptr=(unsigned short *) (aux_buff);
	*shptr = htons(this->CMD_get());aux_buff +=2;
	// EXTRAFRAME needs a special treatment, user defined data
	// unsigned char array to store the data
	int ptr = 0;
	for(ptr ; ptr< this->FRAMESIZE_get()-18;ptr++){
		aux_buff[ptr]=this->EXTRAFRAME[ptr];
	}
	aux_buff = *buff + (this->FRAMESIZE_get()-2);
	shptr=(unsigned short *) (aux_buff);
	// Compute CRC from current frame
	unsigned short crc_aux = this->Calc_CRC(*buff,this->FRAMESIZE_get()-2);
	this->CHK_set(crc_aux);
	*shptr = htons(this->CHK_get());
	return (this->FRAMESIZE_get());
}

