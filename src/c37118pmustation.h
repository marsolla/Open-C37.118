#ifndef c37118pmustation_h
#define c37118pmustation_h

#include "c37118.h"

/**
* C37.118-2011 PMU STATION SUB-FRAME CLASS
* Methods, Get/Set for each field to populate a PMU Station
* Note: Used to map the current PMU and Others PMUs over Network
*       
*/
class PMU_Station : public C37118{

	public: 
	PMU_Station(string name,short idcode,bool FREQ_TYPE , bool ANALOG_TYPE, bool PHASOR_TYPE, bool COORD_TYPE);	
	PMU_Station();	
		
	/**
	* SET Methods
	*/
	void STN_set(string station_name);
	void IDCODE_set(short code);
	void FORMAT_set(bool FREQ_TYPE , bool ANALOG_TYPE, bool PHASOR_TYPE, bool COORD_TYPE );
	void FORMAT_set(unsigned short FORMAT_WORD);
	
	void PHNMR_set(unsigned short number);
	void ANNMR_set(unsigned short number);
	void DGNMR_set(unsigned short number);
	
	void PHASOR_add(string name,unsigned long factor,unsigned int type);
	void PHASOR_add(string name,unsigned int type);
	void PHASOR_VALUE_set(Complex value, unsigned int pos);
	
	
	void ANALOG_add(string name, long factor,unsigned int type);
	void ANALOG_add(string name, unsigned int type);
	void ANALOG_VALUE_set(float value, unsigned int pos);

	void DIGITAL_add(vector<string> name, unsigned short normal, unsigned short valid);
	void DIGITAL_VALUE_set(bool value, unsigned int channel, unsigned int pos);
	void DIGITAL_VALUE_set(vector<bool> value, unsigned int channel);
	
	void FNOM_set(bool Freq_Nom);

	void CFGCNT_inc();
	void CFGCNT_set(unsigned short value);
	void STAT_set(unsigned short value);
	
	void FREQ_set(float value);
	void DFREQ_set(float value);

	/**
	* GET Methods
	*/
	string STN_get();
	unsigned short IDCODE_get();
	bool FORMAT_COORD_get();
	bool FORMAT_PHASOR_TYPE_get();
	bool FORMAT_ANALOG_TYPE_get();
	bool FORMAT_FREQ_TYPE_get();
	unsigned short FORMAT_get();	
	unsigned short DGUNIT_NORMAL_get(unsigned int pos);
	unsigned short DGUNIT_VALID_get(unsigned int pos);
	
	//Numbers of phasors, analog and digital channels
	unsigned short PHNMR_get();
	unsigned short ANNMR_get();
	unsigned short DGNMR_get();
	
	Complex PHASOR_VALUE_get(unsigned int pos);
	float ANALOG_VALUE_get(unsigned int pos);
	bool DIGITAL_VALUE_get(unsigned int channel, unsigned int pos);
	vector<bool> DIGITAL_VALUE_get(unsigned int channel);
	
	bool FNOM_get();
	
	unsigned short CFGCNT_get();
	unsigned short STAT_get();
	
	string PH_NAME_get(unsigned int pos);
	string AN_NAME_get(unsigned int pos);
	string DG_NAME_get(unsigned int pos);
	
	unsigned long PHUNIT_get(unsigned int pos);
	unsigned long ANUNIT_get(unsigned int pos);
	unsigned long DGUNIT_get(unsigned int pos);
	
	unsigned long PHFACTOR_get(unsigned int pos);
	unsigned long ANFACTOR_get(unsigned int pos);

	
	float FREQ_get();
	float DFREQ_get();
	
	/**
	* DATA STORE
	*/	
	protected:
		//STATION NAME
		string STN;
		unsigned short IDCODE;
		unsigned short FORMAT;
		unsigned short PHNMR;
		unsigned short ANNMR;
		unsigned short DGNMR;
		
		//CHANNEL NAMES
		vector<string> CHNAM_Phasor;
		vector<string> CHNAM_Analog;
		vector<string> CHNAM_Digital;

		//Channel Values
		vector<Complex> PHASOR_Values;
		vector<float> ANALOG_Values;
		vector< vector<bool> > DIGITAL_Values;

		//CHANNEL UNIT
		vector<unsigned long> PHUNIT; 
		vector<unsigned long> ANUNIT;
		vector<unsigned long> DGUNIT;
	
		unsigned short FNOM;
		unsigned short CFGCNT;
		unsigned short STAT;
	
		float FREQ;
		float DFREQ;

};

#endif
