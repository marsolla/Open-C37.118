#include "c37118.h"
/**
*       IEEE C37.118-2011 to identify the follows fields in a buffer
*       Command Frame
*		Header Frame
*		Configuration Frame 1, 2, 3
*		Data Frame
*/
/**
*  Set SYNC Field from All Frame
*/
void C37118::SYNC_set(unsigned short value){
	this->SYNC = value;	
}
/**
*  Set FRAMESIZE Field from All Frame
*/
void C37118::FRAMESIZE_set(unsigned short value){
	this->FRAMESIZE = value;	
}
/**
*  Set IDCODE Field from All Frame
*/
void C37118::IDCODE_set(unsigned short value){
	this->IDCODE = value;	
}
/**
*  Set SOC Field from All Frame
*/
void C37118::SOC_set(unsigned long value){
	this->SOC = value;	
}
/**
*  Set FRACSEC Field from All Frame
*/
void C37118::FRACSEC_set(unsigned long value){
	this->FRACSEC = value;	
}

/**
*  Set CRC CHECK Field from All Frame
*/
void C37118::CHK_set(unsigned short value){
	this->CHK = value;	
}

/**
*  Get SYNC Field from All Frame
*/
unsigned short C37118::SYNC_get(){
	return this->SYNC;
}
/**
*  Get FRAMESIZE Field from All Frame
*/
unsigned short C37118::FRAMESIZE_get(){
	return this->FRAMESIZE;
}
/**
*  Get IDCODE Field from All Frame
*/
unsigned short C37118::IDCODE_get(){
	return this->IDCODE;
}
/**
*  Get SOC Field from All Frame
*/
unsigned long C37118::SOC_get(){
	return this->SOC;
}
/**
*  Get FRACSEC Field from All Frame
*/
unsigned long C37118::FRACSEC_get(){
	return this->FRACSEC;
}

/**
*  Get CRC CHECK Field from DATA Frame
*/
unsigned short C37118::CHK_get(){
	return this->CHK;
}

/**
* Protected Common Methods, Visible to all classes 
* Compute CRC check FRAME based on C37.118-2011
*/
unsigned short C37118::Calc_CRC(unsigned char* sData, unsigned int iDataLen)
{
  //// CRC-CCITT Calculation
  // f(x) = x^16 + x^12 + x^5 + 1
  //
  // Derived from IEEE Std C37.118-2005 sample code 
  // Example:  cout << "CRC of " << "Arnold" << " = " << Calc_CRC((unsigned char*)"Arnold") << endl;
  unsigned short iCrc = 0xFFFF;   // 0xFFFF is specific for SynchroPhasor Data CRC
  unsigned short iCalc1;
  unsigned short iCalc2;
  unsigned short ii;
  for (ii = 0; ii < iDataLen; ii++)
  {
    iCalc1 = (iCrc >> 8) ^ sData[ii];
    iCrc <<= 8;
    iCalc2 = iCalc1 ^ (iCalc1 >> 4);
    iCrc ^= iCalc2;
    iCalc2 <<= 5;
    iCrc ^= iCalc2;
    iCalc2 <<= 7;
    iCrc ^= iCalc2;
  }
  return iCrc;
}
