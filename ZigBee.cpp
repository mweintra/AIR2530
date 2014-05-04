

#include "ZigBee.h"
#include <stdint.h>

#include "Energia.h"
#include "utility/HAL.h"
#include "utility/utilities.h"
#include "utility/module_errors.h"
#include "utility/zm_phy_spi.h"
#include "utility/module.h"


moduleResult_t result = MODULE_SUCCESS;

//moduleResult_t result = MODULE_SUCCESS;
ZigBeeClass::ZigBeeClass(){

}

void ZigBeeClass::setMRSTpin(uint8_t pin){
	MRSTpin=pin;
}

void ZigBeeClass::setMRDYpin(uint8_t pin){
	MRDYpin=pin;
}
void ZigBeeClass::setSRDYpin(uint8_t pin){
	SRDYpin=pin;
}
void ZigBeeClass::setSPImodule(uint8_t module){
	SPImodule=module;
}

  
  

void ZigBeeClass::init(void) {
	Serial.println("ZigBee Init:");
	displayVersion();

	halInit();
    moduleInit();
    moduleReset();
	displaySysResetInd();

}

void ZigBeeClass::reset(void){

        printf("Module Reset:\n\r");
        result = moduleReset();
        if (result == MODULE_SUCCESS) {
            displaySysResetInd();  // Display the contents of the received SYS_RESET_IND message
        } else {
            printf("ERROR 0x%02X\r\n", result);
       
		}
}

void ZigBeeClass::getMac(void){

	result = zbGetDeviceInfo(DIP_MAC_ADDRESS);
	if (result == MODULE_SUCCESS)
	{
		uint8_t* mac = zmBuf+SRSP_DIP_VALUE_FIELD;
		printf("MAC (as sent, LSB first):");
		printHexBytes(mac, 8);
		
		/* Note: the MAC address comes over the wire in reverse order (LSB first)
		So we swap the order of the bytes so we can display it correctly. */
		
		uint8_t temp[8];
		int i;
		for (i=0; i<8; i++)
		{
			temp[i] = mac[7-i];
		}
		printf("MAC (correct, MSB first):");
		printHexBytes(temp, 8);
		Serial.println();
	}
	else
		printf("ERROR 0x%02X\r\n", result);

}
  void ZigBeeClass::begin(){
	
  }

  void ZigBeeClass::setType(){
	
  }

ZigBeeClass ZigBee;