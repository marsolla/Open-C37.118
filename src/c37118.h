#ifndef c37118_h
#define c37118_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <vector>
#include <complex>
#include <iostream>

/**
* Complex FLOAT type
*/
typedef std::complex<float> Complex;

/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0xAA = Frame Synchronization byte.  Start of Message Character
*/
#define A_SYNC_AA 0xAA     
// SYNC[1] Second byte
// bit[7] = Reserved
// bits[6,5,4] = Frame Type, bits[3,2,1,0] = Version Number (IEEE C35.118-2005 = 0001
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Data Frame byte.
*/
//      r000 = Data Frame
#define A_SYNC_DATA 0x01
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x11 = Header Frame byte. 
*/
//      r001 = Header Frame
#define A_SYNC_HDR 0x11
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Config1 Frame byte. 
*/
//      r010 = Config Frame 1
#define A_SYNC_CFG1 0x21
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Config2 Frame byte.
*/
//      r011 = Config Frame 2
#define A_SYNC_CFG2 0x31
/**
* IEEE Std C37.118-2005 SynchroPhasor Message Prefixes
* 0x01 = Command Frame byte. 
*/
//      r100 = Command Frame
#define A_SYNC_CMD 0x41

/**
* IEEE Std C37.118-2005 SynchroPhasor Bit Type
* PHASOR CHANNEL NAME MEASURE TYPE 
* 0x00 = VOLTAGE
* 0x01 = CURRENT
*/
//PHASOR BIT CONF
enum PHUNIT_Bit {
	VOLTAGE = 0,
	CURRENT = 1,
};

/**
* IEEE Std C37.118-2005 SynchroPhasor Bit Type
* ANALOG CHANNEL NAME MEASURE TYPE 
* 0x00 = SINGLE POINT ON WAVE
* 0x01 = RMS ANALOG INPUT
* 0x02 = PEAK ANALOG INPUT
*/
//ANALOG BIT CONF
enum ANUNIT_Bit {
	SINGLE_POINT_ON_WAVE = 0,
	RMS_ANALOG_INPUT = 1,
	PEAK_ANALOG_INPUT = 2,	
};

/**
* IEEE Std C37.118-2005 SynchroPhasor Bit Type
* PMU NOMINAL FREQUENCY OPERATION 
* 0x00 = 60 Hz
* 0x01 = 50 Hz
*/
// NOMINAL FREQ
enum FREQ_NOM{
	FN_60HZ = 0,
	FN_50HZ = 1,
};

using namespace std;

class C37118 {
	
public :

	/**
	* SET Methods
	*/
	void SYNC_set(unsigned short value);
	void FRAMESIZE_set(unsigned short value);
	void IDCODE_set(unsigned short value);
	void SOC_set(unsigned long value);
	void FRACSEC_set(unsigned long value);
	void CHK_set(unsigned short value);
	
	/**
	* GET Methods
	*/
	unsigned short SYNC_get();
	unsigned short FRAMESIZE_get();
	unsigned short IDCODE_get();
	unsigned long SOC_get();
	unsigned long FRACSEC_get();
	unsigned short CHK_get();

protected:	
	/** 
	* Compute CRC check FRAME based on C37.118-2011
	*/
	unsigned short Calc_CRC(unsigned char* sData, unsigned int iDataLen);

	unsigned short SYNC;         
	unsigned short FRAMESIZE;
	unsigned short IDCODE;
	unsigned long SOC;
	unsigned long FRACSEC;
	unsigned short CHK;
	
};
#endif
