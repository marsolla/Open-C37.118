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
void HEADER_Frame::unpack(unsigned char *buffer){
     	unsigned char *buffer2;
     	unsigned short size_data;
		this->SYNC_set(ntohs(*((unsigned short*)(buffer))));
		this->FRAMESIZE_set(ntohs(*((unsigned short*)(buffer+2))));
		this->IDCODE_set(ntohs(*((unsigned short*)(buffer+4))));
		this->SOC_set(ntohl(*((unsigned long*)(buffer+6))));
		this->FRACSEC_set(ntohl(*((unsigned long*)(buffer+10))));
		buffer2 = buffer + 14; 
		
		size_data = this->FRAMESIZE_get()-16;
		char * cstr = new char [size_data];
		for(int ptr =0 ; ptr< size_data;ptr++){
			cstr[ptr] = buffer2[ptr];
		}
		this->DATA_set(string(cstr));
		
		this->CHK_set(ntohs(*((unsigned short *)(buffer+(this->FRAMESIZE_get()-2)))));
		//printf("Packet Received: %x,%x,%x,%x,%x,%x,%x\n",this->FRAMESIZE_get(),this->IDCODE_get(),this->SOC_get(),this->FRACSEC_get(),this->CMD_get(),this->EXTRAFRAME_get(),this->CHK_get());
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
