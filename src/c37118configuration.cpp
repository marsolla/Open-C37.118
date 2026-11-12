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

#include "c37118configuration.h"

/**
* PUBLIC Methods from CLASS CONFIGURATION FRAME 2, Get/Set for each field
*/

/**
* Method Constructor
* Init the 2 first byte with CONFIGURATION Frame ID
* Add Header Default 0xAA|0x31=Conf2 --> or 0x21 = Conf1
*/

CONFIG_Frame::CONFIG_Frame(){
	//Set header with RUNNING CONFIG (config mode 2)
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_CFG2);
	this->NUM_PMU = 0;
}

/**
*  Set TIME_BASE Field from Configuration Frame
*/
void CONFIG_Frame::TIME_BASE_set(unsigned long value){
	this->TIME_BASE = value;	
}
/**
*  Set NUM_PMU Field from Configuration Frame
*/
void CONFIG_Frame::NUM_PMU_set(unsigned short value){
	this->NUM_PMU = value;	
}
/**
*  Set DATA_RATE Field from Configuration Frame
*/
void CONFIG_Frame::DATA_RATE_set(unsigned short value){
	this->DATA_RATE = value;	
}


/**
*  Get TIME_BASE Field from Configuration Frame
*/
unsigned long CONFIG_Frame::TIME_BASE_get(){
	return this->TIME_BASE;
}
/**
*  Get NUM_PMU Field from Configuration Frame
*/
unsigned short CONFIG_Frame::NUM_PMU_get(){
	return this->NUM_PMU;
}


/**
* ADD a PMU_Station UNIT to PMU List and increase NUM_PMU
*/
void CONFIG_Frame::PMUSTATION_ADD(PMU_Station *PS){
	this->pmu_station_list.push_back(PS);
	this->NUM_PMU++;
}

/**
* GET a PMU_Station UNIT from IDCODE Registered in the system
*/
PMU_Station *CONFIG_Frame::PMUSTATION_GETbyIDCODE(unsigned short idcode){
	int found = 0;
	for (int i =0; i < this->pmu_station_list.size() ; i++){
		if (this->pmu_station_list[i]->IDCODE_get() == idcode){
			found = i;
		}
	}
	
return this->pmu_station_list[found];
}

/**
*  Get DATA_RATE Field from Configuration Frame
*/
unsigned short CONFIG_Frame::DATA_RATE_get(){
	return this->DATA_RATE;
}

/**
* Method unpack_config_frame
* Received a frame buffer, parse all fields until FRAMESIZE has been reached
* Values stored in this class
*/
void CONFIG_Frame::unpack(unsigned char *buffer, unsigned int buffer_len){
	unsigned char *aux_buffer, *aux_buffer2 ;
	const unsigned char *bend = buffer + buffer_len;
	PMU_Station *PS = NULL;
	// Bound every read against the number of bytes actually received. unpack() historically trusted
	// the frame's own FRAMESIZE and per-station counts and walked off the end of short frames; this
	// rejects a frame whose declared contents do not fit in buffer_len.
	#define C37_NEED(p, n) do { if ((p) > bend || (size_t)(bend - (p)) < (size_t)(n)) goto fail; } while(0)

	aux_buffer = buffer;
	aux_buffer2 = buffer;
	C37_NEED(aux_buffer, 2); this->SYNC_set(ntohs(*((unsigned short*)(aux_buffer))));      aux_buffer += 2;
	C37_NEED(aux_buffer, 2); this->FRAMESIZE_set(ntohs(*((unsigned short*)(aux_buffer)))); aux_buffer += 2;
	C37_NEED(aux_buffer, 2); this->IDCODE_set(ntohs(*((unsigned short*)(aux_buffer))));    aux_buffer += 2;
	C37_NEED(aux_buffer, 4); this->SOC_set(ntohl(*((unsigned int*)(aux_buffer))));         aux_buffer += 4;
	C37_NEED(aux_buffer, 4); this->FRACSEC_set(ntohl(*((unsigned int*)(aux_buffer))));     aux_buffer += 4;
	C37_NEED(aux_buffer, 4); this->TIME_BASE_set(ntohl(*((unsigned int*)(aux_buffer))));   aux_buffer += 4;
	C37_NEED(aux_buffer, 2);
	// Scope the body locals (pmu_count, per-station counts) so none are in scope at the fail label;
	// this keeps the goto-based bounds checks well-formed under -Werror.
	{
	unsigned short pmu_count = (ntohs(*((unsigned short*)(aux_buffer))));                  aux_buffer += 2;
	// FOR EACH PMU STATION
	for(int pos = 0; pos < pmu_count ; pos++){
		PS = new PMU_Station();
		C37_NEED(aux_buffer, 16); PS->STN_set(string((const char*)aux_buffer,16));         aux_buffer += 16;
		C37_NEED(aux_buffer, 2);  PS->IDCODE_set(ntohs(*(unsigned short*)(aux_buffer)));   aux_buffer += 2;
		C37_NEED(aux_buffer, 2);  PS->FORMAT_set(ntohs(*(unsigned short*)(aux_buffer)));   aux_buffer += 2;
		C37_NEED(aux_buffer, 2);  unsigned short phasor_n  = ntohs(*(unsigned short*)(aux_buffer)); aux_buffer += 2;
		C37_NEED(aux_buffer, 2);  unsigned short analog_n  = ntohs(*(unsigned short*)(aux_buffer)); aux_buffer += 2;
		C37_NEED(aux_buffer, 2);  unsigned short digital_n = ntohs(*(unsigned short*)(aux_buffer)); aux_buffer += 2;

		// Channel sub-frame: names (16 B each) are at aux_buffer, conversion factors are at
		// aux_buffer2, which starts after the whole name block. Verify the block fits before use.
		size_t chanblock = (size_t)16 * ((size_t)phasor_n + (size_t)analog_n + (size_t)16 * (size_t)digital_n);
		if ((aux_buffer > bend) || ((size_t)(bend - aux_buffer) < chanblock)) goto fail;
		aux_buffer2 = aux_buffer + chanblock;

		// PHASOR
		for(int chan = 0; chan < phasor_n ; chan++){
			C37_NEED(aux_buffer2, 4);
			unsigned long factor_aux = ntohl(*(unsigned int*)(aux_buffer2)); factor_aux &= 0x0FFFFFF;
			unsigned long type_aux   = ntohl(*(unsigned int*)(aux_buffer2)); type_aux &= 0xF000000; type_aux = (type_aux >> 24);
			C37_NEED(aux_buffer, 16); PS->PHASOR_add(string((const char*)aux_buffer,16),factor_aux,type_aux);
			aux_buffer += 16; aux_buffer2 += 4;
		}
		// ANALOG
		for(int chan = 0; chan < analog_n ; chan++){
			C37_NEED(aux_buffer2, 4);
			unsigned long factor_aux = ntohl(*(unsigned int*)(aux_buffer2)); factor_aux &= 0x0FFFFFF;
			unsigned long type_aux   = ntohl(*(unsigned int*)(aux_buffer2)); type_aux &= 0xF000000; type_aux = (type_aux >> 24);
			C37_NEED(aux_buffer, 16); PS->ANALOG_add(string((const char*)aux_buffer,16),factor_aux,type_aux);
			aux_buffer += 16; aux_buffer2 += 4;
		}
		// DIGITAL
		for(int chan = 0; chan < digital_n ; chan++){
			vector<string> aux ;
			for(int bit = 0; bit < 16 ; bit++){
				C37_NEED(aux_buffer, 16); aux.push_back(string((const char*)aux_buffer,16)); aux_buffer += 16;
			}
			C37_NEED(aux_buffer2, 2); unsigned short msb_normal = ntohs(*(unsigned short*)(aux_buffer2)); aux_buffer2 += 2;
			C37_NEED(aux_buffer2, 2); unsigned short lsb_valid  = ntohs(*(unsigned short*)(aux_buffer2)); aux_buffer2 += 2;
			PS->DIGITAL_add(aux,msb_normal,lsb_valid);
		}

		C37_NEED(aux_buffer2, 2); PS->FNOM_set(ntohs(*(unsigned short*)(aux_buffer2)));   aux_buffer2 += 2;
		C37_NEED(aux_buffer2, 2); PS->CFGCNT_set(ntohs(*(unsigned short*)(aux_buffer2))); aux_buffer2 += 2;

		aux_buffer = aux_buffer2;
		this->PMUSTATION_ADD(PS);
		PS = NULL; // ownership transferred to the station list
	}

	C37_NEED(aux_buffer2, 2); this->DATA_RATE_set(ntohs(*((unsigned short*)(aux_buffer2)))); aux_buffer2 += 2;
	C37_NEED(aux_buffer2, 2); this->CHK_set(ntohs(*(unsigned short *)(aux_buffer2)));
	}
	#undef C37_NEED
	return;
fail:
	if (PS) delete PS; // free the station being parsed when a malformed frame is rejected
	return;
}

/**
* Method pack_config_buffer
* Mount a new config frame based on values store in the the class,
* special short and long pointers are needs to populate the buffer.
* total frame size = 24 + NUM_PMU*(26 + 16*(PHNMR+ANNMR+16*DGNMR) 
*                                  + 4*PHNMR + 4*ANNMR + 4*DGNMR + 4);
*/
unsigned short CONFIG_Frame::pack(unsigned char **buff){
	// total frame size = 24 + NUM_PMU*(26 + 16*(PHNMR+ANNMR+16*DGNMR) + 4*PHNMR + 4*ANNMR + 4*DGNMR + 4);
	unsigned short size = 24;

	// compute channel numbers foreach exists pmu
	for(int i =0; i<this->NUM_PMU_get();i++){
		size += 30; // 26 + 4
		size += 16*(pmu_station_list[i]->PHNMR_get() + pmu_station_list[i]->ANNMR_get() + (16*pmu_station_list[i]->DGNMR_get())  );
		size += 4*pmu_station_list[i]->PHNMR_get() + 4*pmu_station_list[i]->ANNMR_get() + 4*pmu_station_list[i]->DGNMR_get();
	}
	
	// set frame size
	this->FRAMESIZE_set(size);

	//pointers to buffer
	unsigned char *aux_buff;
	unsigned short *shptr;
	unsigned long *lptr;
	
	//buff size reserved
	*buff = (unsigned char *) malloc (this->FRAMESIZE*sizeof(char));
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
	lptr=(unsigned long *) (aux_buff);
	*lptr = htonl(this->TIME_BASE_get()); aux_buff +=4;
	shptr=(unsigned short *) (aux_buff);	
	*shptr = htons(this->NUM_PMU_get());aux_buff +=2;
	
	// For each pmu station
	for(int i = 0; i<this->NUM_PMU_get() ; i++){
		char * cstr = new char [16];
		
		//Get name string and convert to char string
		strcpy (cstr, this->pmu_station_list[i]->STN_get().c_str());
		for(int ptr =0 ; ptr< 16;ptr++){
			aux_buff[ptr]=cstr[ptr];
		}
		aux_buff +=16;
		//Get another fields
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->pmu_station_list[i]->IDCODE_get());aux_buff +=2;
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->pmu_station_list[i]->FORMAT_get());aux_buff +=2;
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->pmu_station_list[i]->PHNMR_get());aux_buff +=2;
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->pmu_station_list[i]->ANNMR_get());aux_buff +=2;
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->pmu_station_list[i]->DGNMR_get());aux_buff +=2;
		
		// Channels Name
		//Phasor
		for(int j =0 ;j< this->pmu_station_list[i]->PHNMR_get() ; j++){
			strcpy (cstr, this->pmu_station_list[i]->PH_NAME_get(j).c_str());
			for(int ptr =0 ; ptr< 16;ptr++){
				aux_buff[ptr]=cstr[ptr];
			}
			aux_buff +=16;
		}
		//Analog
		for(int j=0 ; j < this->pmu_station_list[i]->ANNMR_get() ; j++){
			strcpy (cstr, this->pmu_station_list[i]->AN_NAME_get(j).c_str());
			for(int ptr =0 ; ptr< 16;ptr++){
				aux_buff[ptr]=cstr[ptr];
			}
			aux_buff +=16;
		}
		//Digital
		for(int j =0 ; j < 16*this->pmu_station_list[i]->DGNMR_get() ; j++){
			strcpy (cstr, this->pmu_station_list[i]->DG_NAME_get(j).c_str());
			for(int ptr =0 ; ptr< 16;ptr++){
				aux_buff[ptr]=cstr[ptr];
			}
			aux_buff +=16;
		}

		//UNIT FACTOR
		//Phasor
		//cout << "PHNMR:" << this->pmu_station_list[i]->PHNMR_get() << endl;
		for(int j=0; j< this->pmu_station_list[i]->PHNMR_get();j++){
			lptr=(unsigned long *) (aux_buff);
			
			//cout <<"PHUNIT:" <<this->pmu_station_list[i]->PHUNIT_get(j) << endl;
			*lptr = htonl(this->pmu_station_list[i]->PHUNIT_get(j)); aux_buff +=4;
		} 
		//Analog
		for(int j=0; j< this->pmu_station_list[i]->ANNMR_get();j++){
			lptr=(unsigned long *) (aux_buff);
			*lptr = htonl(this->pmu_station_list[i]->ANUNIT_get(j)); aux_buff +=4;
		}
		//Digital
		for(int j=0; j< this->pmu_station_list[i]->DGNMR_get();j++){
			lptr=(unsigned long *) (aux_buff);
			*lptr = htonl(this->pmu_station_list[i]->DGUNIT_get(j)); aux_buff +=4;
		}

		//FNOM
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->pmu_station_list[i]->FNOM_get());aux_buff +=2;
	
		// CFGCNT
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->pmu_station_list[i]->CFGCNT_get());aux_buff +=2;
	
	}// END FOR OF PMU STATIONS
	
	//DATA RATE
	shptr=(unsigned short *) (aux_buff);	
	*shptr = htons(this->DATA_RATE_get());aux_buff +=2;

	// Compute CRC from current frame
	unsigned short crc_aux = Calc_CRC(*buff,this->FRAMESIZE_get()-2);
	this->CHK_set(crc_aux);
	// get computed CRC 
	shptr=(unsigned short *) (aux_buff);
	*shptr = htons(this->CHK_get());
	
	// return # of bytes writed
	return (this->FRAMESIZE_get());
}

/**
* PUBLIC Methods from CLASS CONFIGURATION FRAME 1, Get/Set for each field
*/

/**
* Method Constructor - SAME OF CONFIGURATION FRAME 2, BUT...
* Init the 2 first byte with CONFIGURATION 1 Frame ID 
* Add Header Default 0xAA|0x21 = Conf1
*/

CONFIG_1_Frame::CONFIG_1_Frame(){
	//Set header with RUNNING CONFIG (config mode 1)
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_CFG1);
	this->NUM_PMU = 0;
}

