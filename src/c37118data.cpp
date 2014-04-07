#include "c37118data.h"

/**
* PUBLIC Methods from CLASS DATA FRAME, Get/Set for each field
*/

/**
* Method Constructor
* Init the 2 first byte with DATA Frame ID
* Add Header of packet 0xAA|0x01
*/
DATA_Frame::DATA_Frame(CONFIG_Frame *cfg){
	this->SYNC = (A_SYNC_AA << 8 | A_SYNC_DATA );
	this->associate_current_config = cfg;
}

/**
* Method pack_data_buffer
* Mount a new data frame based on values store in the the class,
* special short and long pointers are needs to populate the buffer.
* total frame size is different because the UNIT and Quantity of Channels are
* variable. All considered.
*/

typedef union _data {
  float f;
  char  s[4];
} myData;

unsigned short DATA_Frame::pack(unsigned char **buff){
	myData aux_conv;

	unsigned short size = 14;
	// compute channel numbers foreach exists pmu
	for(int i =0; i<this->associate_current_config->NUM_PMU_get();i++){

		//STAT
		size +=2;
	
		//PASHOR
		// Data mode = float 32 bits
		if(this->associate_current_config->pmu_station_list[i]->FORMAT_PHASOR_TYPE_get()){
			size += 8*this->associate_current_config->pmu_station_list[i]->PHNMR_get();
		}
		// Data mode = Integer 16 bits
		else{
			size += 4*this->associate_current_config->pmu_station_list[i]->PHNMR_get();
		}
				
		//FREQ and DFREQ
		// Data mode = float 32 bits
		if(this->associate_current_config->pmu_station_list[i]->FORMAT_FREQ_TYPE_get()){
			size += 4;//FREQ
			size += 4;//DFREQ
		}
		// Data mode = Integer 16 bits
		else{
			size += 2;//FREQ
			size += 2;//DFREQ
		}
		
		//ANALOG
		// Data mode = float 32 bits
		if(this->associate_current_config->pmu_station_list[i]->FORMAT_ANALOG_TYPE_get()){
			size += 4*this->associate_current_config->pmu_station_list[i]->ANNMR_get();
		}
		// Data mode = Integer 16 bits
		else{
			size += 2*this->associate_current_config->pmu_station_list[i]->ANNMR_get();
		}
		
		//DIGITAL
		size += 2*this->associate_current_config->pmu_station_list[i]->DGNMR_get();
		
	}// end for pmu station
	
	//Add CRC Empty Field
	size += 2;
	
	//set frame size
	this->FRAMESIZE_set(size);

	//pointers to buffer
	unsigned char *aux_buff;
	unsigned short *shptr;
	unsigned long *lptr;
	unsigned char *fptr;
	
	//buff size reserved
	if(*buff == NULL){
		*buff = (unsigned char *) malloc (this->FRAMESIZE_get()*sizeof(char));
	}
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

	// For each pmu station
	for(int i = 0; i< this->associate_current_config->NUM_PMU_get() ; i++){
		
		//Get Status Value
		shptr=(unsigned short *) (aux_buff);	
		*shptr = htons(this->associate_current_config->pmu_station_list[i]->STAT_get());
		aux_buff +=2;
		
		//Channels Value
		
		//Foreach Phasors
		for(int j = 0; j< this->associate_current_config->pmu_station_list[i]->PHNMR_get(); j++){
			// Data mode = float
			if(this->associate_current_config->pmu_station_list[i]->FORMAT_PHASOR_TYPE_get()){
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()){
					//Float Converter Struct			
					aux_conv.f = abs(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					// Copy float to buffer
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++; 
					//Float Converter Struct
					aux_conv.f = arg(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					// Copy float to buffer
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++;
				}
				// Coord Recta
				else{
					//Float Converter Struct
					aux_conv.f = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).real();
					// Copy float to buffer
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++; 
					
					//Float Converter Struct
					aux_conv.f = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).imag();
					// Copy float to buffer
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[3]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[2]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[1]; aux_buff++;
					fptr=(unsigned char *) (aux_buff);
					*fptr = aux_conv.s[0]; aux_buff++; 
				}
			}
			// Data mode = Integer 16 bits
			else{
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()){
					shptr=(unsigned short *) (aux_buff);
					float aux = abs(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					unsigned short aux2 = (unsigned short) (aux*1E5/this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					*shptr = htons(aux2); 
					aux_buff +=2;
					
					shptr=(unsigned short *) (aux_buff);
					aux = arg(this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j));
					 aux2 = (unsigned short) (aux*1E4);
					*shptr = htons(aux2); 
					aux_buff +=2;
				}
				// Coord Recta
				else{
					shptr=(unsigned short *) (aux_buff);
					float aux = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).real();
					unsigned short aux2 = (unsigned short) (aux*1E5/this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					*shptr = htons(aux2); 
					aux_buff +=2;
					
					shptr=(unsigned short *) (aux_buff);
					aux = this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_get(j).imag();
					 aux2 = (unsigned short) (aux*1E5/this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					*shptr = htons(aux2); 
					aux_buff +=2;
				}
			}// end data mode
		}// end phasors
		
		
		//FREQ e DFREQ
		// Data mode = float
		if(this->associate_current_config->pmu_station_list[i]->FORMAT_FREQ_TYPE_get()){
				
			aux_conv.f = this->associate_current_config->pmu_station_list[i]->FREQ_get();
			// Copy float to buffer
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[3]; aux_buff++;
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[2]; aux_buff++;
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[1]; aux_buff++;
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[0]; aux_buff++; 

			aux_conv.f = this->associate_current_config->pmu_station_list[i]->DFREQ_get();
			// Copy float to buffer
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[3]; aux_buff++;
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[2]; aux_buff++;
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[1]; aux_buff++;
			fptr=(unsigned char *) (aux_buff);
			*fptr = aux_conv.s[0]; aux_buff++; 
		}
		
		// Data mode = Integer 16 bits
		else{
			shptr=(unsigned short *) (aux_buff);
			float aux = this->associate_current_config->pmu_station_list[i]->FREQ_get();
			if(this->associate_current_config->pmu_station_list[i]->FNOM_get())
				aux-=50.0;
			else
				aux-=60.0;
				
			unsigned short aux2 = (unsigned short) (aux*1000);
			*shptr = htons(aux2); 
			aux_buff +=2;
			
			//Scale *100
			shptr=(unsigned short *) (aux_buff);
			aux = this->associate_current_config->pmu_station_list[i]->DFREQ_get();
			aux2 = (unsigned short) (aux*100);
			*shptr = htons(aux2); 
			aux_buff +=2;
		}
			
		//Foreach ANALOGS
		for(int j = 0; j< this->associate_current_config->pmu_station_list[i]->ANNMR_get(); j++){
			// Data mode = float
			if(this->associate_current_config->pmu_station_list[i]->FORMAT_ANALOG_TYPE_get()){
				aux_conv.f = this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_get(j);
				// Copy float to buffer
				fptr=(unsigned char *) (aux_buff);
				*fptr = aux_conv.s[3]; aux_buff++;
				fptr=(unsigned char *) (aux_buff);
				*fptr = aux_conv.s[2]; aux_buff++;
				fptr=(unsigned char *) (aux_buff);
				*fptr = aux_conv.s[1]; aux_buff++;
				fptr=(unsigned char *) (aux_buff);
				*fptr = aux_conv.s[0]; aux_buff++;
			}
			// Data mode = Integer 16 bits
			else{
				shptr=(unsigned short *) (aux_buff);
				float aux = this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_get(j);
				unsigned short aux2 = (unsigned short)(aux); //this->associate_current_config->pmu_station_list[i]->ANFACTOR_get(j));
				*shptr = htons(aux2); 
				aux_buff +=2;
	
			}// end data mode
		}// end analog
		
		//Foreach DIGITAL
		for(int j = 0; j< this->associate_current_config->pmu_station_list[i]->DGNMR_get(); j++){
			shptr=(unsigned short *) (aux_buff);
			unsigned short aux = 0;
			for(int k = 0; k < 16 ; k++){	
				aux |= (this->associate_current_config->pmu_station_list[i]->DIGITAL_VALUE_get(j,k) << k);
			
			}
			shptr=(unsigned short *) (aux_buff);
			*shptr = htons(aux); 
			aux_buff +=2;
		}
	}//foreach PMU STATION
	
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
* Method unpack_data_frame
* Received a frame buffer, parse all fields until FRAMESIZE has been reached
* Values stored in this class
*/
void DATA_Frame::unpack(unsigned char *buffer){
    	unsigned char *aux_buffer, *aux_buffer2 ;
    	myData aux_conv1,aux_conv2;
    	unsigned short aux1, aux2;
    	aux_buffer = buffer;
	this->SYNC_set(ntohs(*((unsigned short*)(aux_buffer))));
	aux_buffer +=2;
	this->FRAMESIZE_set(ntohs(*((unsigned short*)(aux_buffer))));
	aux_buffer +=2;
	this->IDCODE_set(ntohs(*((unsigned short*)(aux_buffer))));
	aux_buffer +=2;
	this->SOC_set(ntohl(*((unsigned long*)(aux_buffer))));
	aux_buffer +=4;
	this->FRACSEC_set(ntohl(*((unsigned long*)(aux_buffer))));
	aux_buffer +=4;
    	
	// For each pmu station
	for(int i = 0; i< this->associate_current_config->NUM_PMU_get() ; i++){
	
	//Get Status Value
	this->associate_current_config->pmu_station_list[i]->STAT_set(ntohs(*((unsigned short*)(aux_buffer))));
	aux_buffer +=2;	
		//Foreach Phasors
		for(int j = 0; j< this->associate_current_config->pmu_station_list[i]->PHNMR_get(); j++){
			// Data mode = float
			if(this->associate_current_config->pmu_station_list[i]->FORMAT_PHASOR_TYPE_get()){
				//Float Converter Struct
				aux_conv1.s[3] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[2] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[1] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[0] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				
				aux_conv2.s[3] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv2.s[2] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv2.s[1] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv2.s[0] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()){
					//cout << "AUX1: " << aux1 << endl;
				//cout << "AUX2: " << aux2 << endl;
				//cout << "Polar ANG: " << aux_conv2.f << endl;
				
					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(polar((float)aux_conv1.f,(float)aux_conv2.f),j);
				}
				// Coord Recta
				else{
					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(Complex(aux_conv1.f,aux_conv2.f),j);
				}
			}
			// Data mode = Integer 16 bits
			else{
				aux1 = ntohs(*((unsigned short*)(aux_buffer)));
				aux_buffer +=2;
				aux2 = ntohs(*((unsigned short*)(aux_buffer)));
				aux_buffer +=2;
				unsigned long aux, aux_b ;			
				// Coord Polar
				if (this->associate_current_config->pmu_station_list[i]->FORMAT_COORD_get()){
					aux = (aux1*this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(polar((float)(((float)aux)/1E5),(float)(((float)aux2)/1E4) ),j);  
				}
				// Coord Recta
				else{
					aux = (aux1*this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					aux_b = (aux2*this->associate_current_config->pmu_station_list[i]->PHFACTOR_get(j));
					this->associate_current_config->pmu_station_list[i]->PHASOR_VALUE_set(Complex((((float)aux)/1E5),(((float)aux_b)/1E5)),j);
				}
				
				
			}// end data mode
		}// end phasors
		
		
		//FREQ e DFREQ
		// Data mode = float
		if(this->associate_current_config->pmu_station_list[i]->FORMAT_FREQ_TYPE_get()){
			//Float Converter Struct
			aux_conv1.s[3] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;
			aux_conv1.s[2] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;
			aux_conv1.s[1] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;
			aux_conv1.s[0] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[3] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[2] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[1] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;
			aux_conv2.s[0] = (*((unsigned char *)(aux_buffer)));
			aux_buffer++;	

			this->associate_current_config->pmu_station_list[i]->FREQ_set(aux_conv1.f);
			this->associate_current_config->pmu_station_list[i]->DFREQ_set(aux_conv2.f);
		}
		
		// Data mode = Integer 16 bits
		else{
			
			aux1 = ntohs(*((unsigned short*)(aux_buffer)));
			aux_buffer +=2;
			aux2 = ntohs(*((unsigned short*)(aux_buffer)));
			aux_buffer +=2;

			if(this->associate_current_config->pmu_station_list[i]->FNOM_get())
				this->associate_current_config->pmu_station_list[i]->FREQ_set(50.0+((float)aux1)/1000);
			else
				this->associate_current_config->pmu_station_list[i]->FREQ_set(60.0+((float)aux1)/1000);
				
			this->associate_current_config->pmu_station_list[i]->DFREQ_set(((float)aux2)/100);
		
		}
			
		//Foreach ANALOGS
		for(int j = 0; j< this->associate_current_config->pmu_station_list[i]->ANNMR_get(); j++){
			// Data mode = float
			if(this->associate_current_config->pmu_station_list[i]->FORMAT_ANALOG_TYPE_get()){
				aux_conv1.s[3] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[2] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[1] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				aux_conv1.s[0] = (*((unsigned char *)(aux_buffer)));
				aux_buffer++;
				
				this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_set(aux_conv1.f,j);
				
				
			}
			// Data mode = Integer 16 bits
			else{
				aux1 = ntohs(*((unsigned short*)(aux_buffer)));
				aux_buffer +=2;
				this->associate_current_config->pmu_station_list[i]->ANALOG_VALUE_set(aux1,j);
			}// end data mode
		}// end analog
		
		//Foreach DIGITAL
		for(int j = 0; j< this->associate_current_config->pmu_station_list[i]->DGNMR_get(); j++){
			aux1 = ntohs(*((unsigned short*)(aux_buffer)));
			aux_buffer +=2;
			//cout<<"AUX: "<<aux1<<endl;
			for(int k = 0; k < 16 ; k++){	
				if(((aux1>>k)&0x0001) == true){
					this->associate_current_config->pmu_station_list[i]->DIGITAL_VALUE_set(true,j,k);
				}else{
					this->associate_current_config->pmu_station_list[i]->DIGITAL_VALUE_set(false,j,k);
				}
			
			}
			
		}
		
	}// foreach PMU STATION
	
	// CRC (PREVIOUS COMPUTED in PACK/UNPACK Methods)		
	this->CHK_set(ntohs(*(unsigned short *)(aux_buffer)));
}



