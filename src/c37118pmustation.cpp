#include "c37118pmustation.h"

/**
* PUBLIC Methods from CLASS PMU STATION Fields, Get/Set for each field
*/
/**
* Method Constructor
* Init with name, idcode and format pre defined
*/
PMU_Station::PMU_Station(string name,short idcode,bool FREQ_TYPE , bool ANALOG_TYPE, bool PHASOR_TYPE, bool COORD_TYPE){
    this->STN_set(name);  
    this->IDCODE_set(idcode);
    this->FORMAT_set(FREQ_TYPE,ANALOG_TYPE,PHASOR_TYPE,COORD_TYPE);
    this->PHNMR = 0;
    this->ANNMR = 0;
    this->DGNMR = 0;
}

/**
* Method Constructor
* Empty, when used to receive a config frame.
* Usualy used to receive a frame config from others PMU/PDC devices
*/
// Used to received a config_frame - Like PDC
PMU_Station::PMU_Station(){
    this->PHNMR = 0;
    this->ANNMR = 0;
    this->DGNMR = 0;
}


/**
*  Set FORMAT Field BYTES for each PMU Unit from Configuration Frame
*/
void PMU_Station::FORMAT_set(bool FREQ_TYPE , bool ANALOG_TYPE, bool PHASOR_TYPE, bool COORD_TYPE ){
	//if true = float or polar, false = integer or rectang. 
	this->FORMAT = 0;
	this->FORMAT |= COORD_TYPE;
	this->FORMAT |= (PHASOR_TYPE << 1);
	this->FORMAT |= (ANALOG_TYPE << 2);
	this->FORMAT |= (FREQ_TYPE << 3);
}

/**
*  Set FORMAT Field WORD for each PMU Unit from Configuration Frame
*  Usualy used to receive a frame config from others PMU/PDC devices
*/
void PMU_Station::FORMAT_set(unsigned short FORMAT_WORD){
	this->FORMAT = FORMAT_WORD;
}

/**
*  Set STATION NAME Field to just ONE PMU Station
*/
void PMU_Station::STN_set(string station_name){
	// 16 bytes
	station_name.append(16,' ');
	this->STN = station_name.substr(0,16);

}
/**
*  Set IDCODE Field to just ONE PMU Station
*/
void PMU_Station::IDCODE_set(short code){
	this->IDCODE = code;
}

/**
*  Set PHASORS CHANNEL NUMBER Field to just ONE PMU Station
*/
void PMU_Station::PHNMR_set(unsigned short number){
	this->PHNMR = number;
}

/**
*  Set ANALOG CHANNEL NUMBER Field to just ONE PMU Station
*/
void PMU_Station::ANNMR_set(unsigned short number){
	this->ANNMR = number;
}

/**
*  Set DIGITAL CHANNEL NUMBER Field to just ONE PMU Station
*/
void PMU_Station::DGNMR_set(unsigned short number){
	this->DGNMR = number;
}


/**
*  Set PHASOR CHANNEL NAME, (TYPE|FACTOR) Field to just ONE PMU Station
*  Using factor represented in 24 bits user defined
*/
void PMU_Station::PHASOR_add(string name,unsigned long factor,unsigned int type){
	name.append(16,' ');
	this->CHNAM_Phasor.push_back(name.substr(0,16));
	//cout << "Type:" << type << endl;
	//cout << "Factor:" << factor << endl;
	//cout << "Mascara:"<< (factor & 0x0FFFFFF) << endl;
	//cout << "Shift:"<< (type << 24)  << endl;
	this->PHUNIT.push_back((type << 24) | (factor & 0x0FFFFFF) );
	this->PHNMR++;
	this->PHASOR_Values.push_back(Complex(0,0));
}

/**
*  Set PHASOR CHANNEL NAME, (TYPE|FACTOR=1) Field to just ONE PMU Station
*  Using factor=1 when represented in float number
*/
void PMU_Station::PHASOR_add(string name,unsigned int type){
	name.append(16,' ');
	this->CHNAM_Phasor.push_back(name.substr(0,16));
	this->PHUNIT.push_back((type << 24)| 0x01 );
	this->PHNMR++;
	this->PHASOR_Values.push_back(Complex(0,0));
}


/**
*  Set ANALOG CHANNEL NAME, (TYPE|FACTOR) Field to just ONE PMU Station
*  Using factor represented in 24 bits user defined
*/
void PMU_Station::ANALOG_add(string name, long factor,unsigned int type){
	name.append(16,' ');
	this->CHNAM_Analog.push_back(name.substr(0,16));
	this->ANUNIT.push_back((type << 24) | (factor & 0x0FFFFFF) );
	this->ANNMR++;
	this->ANALOG_Values.push_back(0.0);

}
/**
*  Set ANALOG CHANNEL NAME, (TYPE|FACTOR=1) Field to just ONE PMU Station
*  Using factor=1 when represented in float number
*/
void PMU_Station::ANALOG_add(string name, unsigned int type){
	name.append(16,' ');
	this->CHNAM_Analog.push_back(name.substr(0,16));
	this->ANUNIT.push_back((type << 24) | 0x01 );
	this->ANNMR++;
	this->ANALOG_Values.push_back(0.0);

}


/**
*  Set 16 x DIGITAL CHANNEL NAME, (NORMAL|VALID) Field to just ONE PMU Station
*  NORMAL = MSB Bytes and VALID is LSB Bytes
*/
void PMU_Station::DIGITAL_add(vector<string> name, unsigned short normal, unsigned short valid){
	for(int i=0; i< name.size() ; i++){ 
		name[i].append(16,' ');
		this->CHNAM_Digital.push_back(name[i].substr(0,16));
	}
	this->DGUNIT.push_back((normal << 16) | (valid));
	this->DGNMR++;
	vector<bool> aux_word(16);
	this->DIGITAL_Values.push_back(aux_word);
}

/**
*  Set NOMINAL FREQUENCY Field to just ONE PMU Station
*  50HZ = 1, 60HZ = 0
*/
void PMU_Station::FNOM_set(bool Freq_Nom){
	this->FNOM = Freq_Nom;	
}

/**
*  Increment CONFIG COUNTER Field to just ONE PMU Station
*  plus 1
*/
void PMU_Station::CFGCNT_inc(){
	this->CFGCNT ++;	
}

/**
*  Set CONFIG COUNTER Field to just ONE PMU Station
*/
void PMU_Station::CFGCNT_set(unsigned short value){
	this->CFGCNT = value;	
}

/**
*  Set STAT Field to just ONE PMU Station
*  Current PMU Station Status Field 
*  Used in DATA FRAME too
*/
void PMU_Station::STAT_set(unsigned short value){
	this->STAT = value;	
}

/**
*  Set PHASOR VALUE IN PHASOR CHANNEL NUMBER = POS to just ONE PMU Station
*  Used in DATA FRAME too
*/
void PMU_Station::PHASOR_VALUE_set(Complex value, unsigned int pos){
	this->PHASOR_Values[pos] = value;
}

/**
*  Set ANALOG VALUE IN ANALOG CHANNEL NUMBER = POS to just ONE PMU Station
*  Used in DATA FRAME too
*/
void PMU_Station::ANALOG_VALUE_set(float value, unsigned int pos){
	this->ANALOG_Values[pos] = value;
}

/**
*  Set DIGITAL BIT VALUE, IN BIT = POS OF ONE DIGITAL CHANNEL to just ONE PMU Station
*  Used in DATA FRAME too
*/
void PMU_Station::DIGITAL_VALUE_set(bool value, unsigned int channel, unsigned int pos){
	this->DIGITAL_Values[channel][pos] = value;
}

/**
*  Set DIGITAL WORD VALUE (16 bits), IN ONE DIGITAL CHANNEL to just ONE PMU Station
*  Used in DATA FRAME too
*/
void PMU_Station::DIGITAL_VALUE_set(vector<bool> value, unsigned int channel){
	this->DIGITAL_Values[channel]= value;
}

/**
*  Set FREQUENCY VALUE Field to just ONE PMU Station
*  Used in DATA FRAME too
*/
void PMU_Station::FREQ_set(float value){
	this->FREQ = value;
}

/**
*  Set d_FREQUENCY/dt VALUE Field to just ONE PMU Station
*  Used in DATA FRAME too
*/
void PMU_Station::DFREQ_set(float value){
	this->DFREQ = value;
}

/**
*  Get STATION NAME Field to just ONE PMU Station
*/
string PMU_Station::STN_get(){
	return(this->STN);
}

/**
*  Get IDCODE Field to just ONE PMU Station
*/
unsigned short PMU_Station::IDCODE_get(){
	return(this->IDCODE);
}

/**
*  Get FORMAT Field WORD to just ONE PMU Station
*/
unsigned short PMU_Station::FORMAT_get(){
	return(this->FORMAT);
}

/**
*  Get FORMAT Field COORDINATE BIT to just ONE PMU Station
*  0 = phasor real + imaginary (rectangular)
*  1 = magnitude + angle (polar)
*/
bool PMU_Station::FORMAT_COORD_get(){
	return(this->FORMAT & 0x01);
}
/**
*  Get FORMAT Field PHASOR TYPE BIT to just ONE PMU Station
*  0 = 16 bits integer
*  1 = float point
*/
bool PMU_Station::FORMAT_PHASOR_TYPE_get(){
	return((this->FORMAT & 0x02) >> 1 );
}
/**
*  Get FORMAT Field ANALOG TYPE BIT to just ONE PMU Station
*  0 = 16 bits integer
*  1 = float point
*/
bool PMU_Station::FORMAT_ANALOG_TYPE_get(){
	return((this->FORMAT & 0x04) >> 2) ;
}
/**
*  Get FORMAT Field FREQ TYPE BIT to just ONE PMU Station
*  0 = 16 bits integer
*  1 = float point
*/
bool PMU_Station::FORMAT_FREQ_TYPE_get(){
	return((this->FORMAT & 0x08) >> 3);
}
/**
*  Get DIGITAL NORMAL STATUS INPUTS
*  0 = Normal
*  1 = Problem
*/
unsigned short PMU_Station::DGUNIT_NORMAL_get(unsigned int pos){
	return (this->DGUNIT[pos] >> 16);
}

/**
*  Get DIGITAL CURRENT VALID INPUTS
*  0 = Invalid
*  1 = Valid
*/
unsigned short PMU_Station::DGUNIT_VALID_get(unsigned int pos){
	return (this->DGUNIT[pos] & 0x00FF);
}

/**
*  Get Numbers of PHASORS
*/
//Numbers of phasors, analog and digital channels
unsigned short PMU_Station::PHNMR_get(){
	 return (this->PHNMR);
}
/**
*  Get Numbers of ANALOGS
*/
unsigned short PMU_Station::ANNMR_get(){
	return (this->ANNMR);
}
/**
*  Get Numbers of DIGITAL
*/
unsigned short PMU_Station::DGNMR_get(){
	return (this->DGNMR);
}

//Names os Channels
/**
*  Get Name of PHASORS Channel Input
*/
string PMU_Station::PH_NAME_get(unsigned int pos){
	return (this->CHNAM_Phasor[pos]);
}
/**
*  Get Name of ANALOGS Channel Input
*/
string PMU_Station::AN_NAME_get(unsigned int pos){
	return (this->CHNAM_Analog[pos]);
}
/**
*  Get Name of DIGITAL Channel Input
*/
string PMU_Station::DG_NAME_get(unsigned int pos){
	return (this->CHNAM_Digital[pos]);
}
//UNIT
/**
*  Get UNIT of PHASORS Channel Input
*/
unsigned long PMU_Station::PHUNIT_get(unsigned int pos){
	return (this->PHUNIT[pos]);
}
/**
*  Get UNIT of ANALOGS Channel Input
*/
unsigned long PMU_Station::ANUNIT_get(unsigned int pos){
	return (this->ANUNIT[pos]);
}
/**
*  Get UNIT of DIGITAL Channel Input
*/
unsigned long PMU_Station::DGUNIT_get(unsigned int pos){
	return (this->DGUNIT[pos]);
}

//Factor
/**
*  Get FACTOR of PHASORS Channel Input
*/
unsigned long PMU_Station::PHFACTOR_get(unsigned int pos){
	return ((this->PHUNIT[pos] & 0x0FFFFFF));
}
/**
*  Get FACTOR of ANALOGS Channel Input
*/
unsigned long PMU_Station::ANFACTOR_get(unsigned int pos){
	return ((this->ANUNIT[pos]& 0x0FFFFFF));
}


// Other remains field
/**
*  Get NOMINAL FREQUENCY Field to just ONE PMU Station
*  50HZ = 1, 60HZ = 0
*/
bool PMU_Station::FNOM_get(){
	return (this->FNOM);
}
/**
*  Get CONFIG COUNTER Field to just ONE PMU Station
*  Indicate how many times the configuration device has changed
*/
unsigned short PMU_Station::CFGCNT_get(){
	return (this->CFGCNT);
}

// DATA
//Status
/**
*  Get STAT Field to just ONE PMU Station
*  Indicate the current status of current PMU Station
*  Used in DATA FRAME too
*/
unsigned short PMU_Station::STAT_get(){
	return (this->STAT);
}

//Channels DATA
/**
*  Get PHASOR VALUE IN PHASOR CHANNEL NUMBER = POS to just ONE PMU Station
*  Used in DATA FRAME too
*/
Complex PMU_Station::PHASOR_VALUE_get(unsigned int pos){
	return (this->PHASOR_Values[pos]);
}

/**
*  Get ANALOG VALUE IN ANALOG CHANNEL NUMBER = POS to just ONE PMU Station
*  Used in DATA FRAME too
*/
float PMU_Station::ANALOG_VALUE_get(unsigned int pos){
	return(this->ANALOG_Values[pos]);
}
/**
*  Get DIGITAL BIT VALUE, IN BIT = POS OF ONE DIGITAL CHANNEL to just ONE PMU Station
*  Used in DATA FRAME too
*/
bool PMU_Station::DIGITAL_VALUE_get(unsigned int channel, unsigned int pos){
	return(this->DIGITAL_Values[channel][pos]);
}
/**
*  Get DIGITAL WORD VALUE (16 bits), IN ONE DIGITAL CHANNEL to just ONE PMU Station
*  Used in DATA FRAME too
*/
vector<bool> PMU_Station::DIGITAL_VALUE_get(unsigned int channel){
	return(this->DIGITAL_Values[channel]);
}

//FREQUENCY DATA
/**
*  Get FREQUENCY VALUE Field to just ONE PMU Station
*  Used in DATA FRAME too
*/
float PMU_Station::FREQ_get(){
	return(this->FREQ);
}
/**
*  Get d_FREQUENCY/dt VALUE Field to just ONE PMU Station
*  Used in DATA FRAME too
*/
float PMU_Station::DFREQ_get(){
	return (this->DFREQ);
}

