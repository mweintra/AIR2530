

#include "ZigBee.h"
#include <stdint.h>

#include "Energia.h"
#include "utility/HAL.h"
#include "utility/utilities.h"
#include "utility/module_errors.h"
#include "utility/zm_phy_spi.h"
#include "utility/module.h"
#include "utility/module_utilities.h"
#include "utility/application_configuration.h"
#include "utility/af.h"
#include "utility/zdo.h"

moduleResult_t result = MODULE_SUCCESS;
struct moduleConfiguration moduleConfiguration = DEFAULT_MODULE_CONFIGURATION_COORDINATOR;
uint16_t operatingRegion=0;



//moduleResult_t result = MODULE_SUCCESS;
ZigBeeClass::ZigBeeClass(){
	#ifdef __MSP430_HAS_EUSCI_B0__
		MRSTpin=P3_0;
		MRDYpin=P3_1;
		SRDYpin=P3_2;
	#endif
	#if defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____) || defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) 
		MRSTpin=PE_0;
		MRDYpin=PA_5;
		SRDYpin=PA_7;
		SPImodule=2;
	#endif
	networkOnline=false;
	sendBufferIndex=0;
	receiveBufferIndex=0;
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

  
  

void ZigBeeClass::begin() {
#ifdef UART_DEBUG
	displayVersion();
	displaySysResetInd();
#endif
	halInit();
    moduleInit();
	moduleReset();
}

void ZigBeeClass::reset(){
        printf("Module Reset:\n\r");
        result = moduleReset();
        if (result == MODULE_SUCCESS) {
            displaySysResetInd();  // Display the contents of the received SYS_RESET_IND message
        } else {
            printf("ERROR 0x%02X\r\n", result);
       
		}
}

void ZigBeeClass::getMac(){

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

void ZigBeeClass::setRegion(uint16_t region){
		operatingRegion=region;
} 
void ZigBeeClass::setChannelMask(uint32_t channelMask){
		moduleConfiguration.channelMask=channelMask;
}  
void ZigBeeClass::setPanID(uint16_t panId){
		moduleConfiguration.panId=panId;
}
void ZigBeeClass::setPollRate(uint16_t endDevicePollRate){
		moduleConfiguration.endDevicePollRate=endDevicePollRate;
}
void ZigBeeClass::setStartupOptions(uint8_t startupOptions){
		moduleConfiguration.startupOptions=startupOptions;
}
void ZigBeeClass::setSecurityMode(uint8_t securityMode){
		moduleConfiguration.securityMode=securityMode;
}
void ZigBeeClass::setSecurityKeys(char* securityKey){
		moduleConfiguration.securityKey=(uint8_t *)securityKey;
}

moduleResult_t ZigBeeClass::start(uint8_t deviceType){
	sendBufferIndex=0;
	receiveBufferIndex=0;
	moduleConfiguration.deviceType=deviceType;
	if ((result = startModule(&moduleConfiguration, GENERIC_APPLICATION_CONFIGURATION,operatingRegion)) != MODULE_SUCCESS)
    {
        /* Module startup failed; display error */
        printf("Module start unsuccessful. Error Code 0x%02X.", result);
		Serial.println(); 
		networkOnline=false;
    }else{
		Serial.println(); 
		Serial.println("Success!"); 
		networkOnline=true;
	}
	return result;
}
  
  moduleResult_t ZigBeeClass::send(uint16_t shortAddress){
	if(networkOnline){
		if (result=afSendData(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, 0, INFO_MESSAGE_CLUSTER, sendBuffer, sendBufferIndex)!=MODULE_SUCCESS){
			networkOnline=false;
		}else{
			sendBufferIndex=0;
		}
	}
	return result;
}
  
moduleResult_t ZigBeeClass::send(uint16_t shortAddress, char* message,uint16_t messageSize ){
	if(networkOnline){
		if (result=afSendData(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, 0, INFO_MESSAGE_CLUSTER, (uint8_t*)message, messageSize)!=MODULE_SUCCESS){
			networkOnline=false;
		}
	}
	return result;
}

bool ZigBeeClass::hasMessage(){
	if(moduleHasMessageWaiting()){
		getMessage();
		receiveBufferIndex=0;
		if (zmBuf[SRSP_LENGTH_FIELD] > 0)
		{
			message_type=(CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]));
		}
		return true;
	}
	return false;
}

moduleResult_t ZigBeeClass::receive(){
	getMessage();
	if (IS_ZDO_END_DEVICE_ANNCE_IND()) {
        displayZdoEndDeviceAnnounce(zmBuf);
    } else { //unknown message, just print out the whole thing

	}
	return 0;
}
 
 void ZigBeeClass::printMessage(){
	displayMessage();
 }

void ZigBeeClass::put(uint8_t data){
	sendBuffer[sendBufferIndex++]=data;
}
void ZigBeeClass::addValue(uint16_t data){
	sendBuffer[sendBufferIndex]=MSB(data);
	sendBuffer[sendBufferIndex+1]=LSB(data);
	sendBufferIndex+=2;
}

void ZigBeeClass::addKVP(uint16_t key, uint16_t value){
	sendBuffer[sendBufferIndex]=MSB(key);
	sendBuffer[sendBufferIndex+1]=LSB(key);
	sendBuffer[sendBufferIndex+2]=MSB(value);
	sendBuffer[sendBufferIndex+3]=LSB(value);	
	sendBufferIndex+=4;
}
 
uint8_t ZigBeeClass::get(uint8_t index){
	return zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+index];
}

uint16_t ZigBeeClass::getValue(){
	uint8_t index=	receiveBufferIndex;
	receiveBufferIndex+=2;
	return (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+index+1],zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+index]));

}
uint16_t ZigBeeClass::getKVP(uint16_t key){
	for (int i=0;i<zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD];i+=4){
		if (key ==(CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+i+1],zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+i]))){
			return (CONVERT_TO_INT(zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+i+3],zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+i+2]));
		}
	}
}
uint8_t ZigBeeClass::getLQI(){
	return zmBuf[AF_INCOMING_MESSAGE_LQI_FIELD];
}

uint8_t ZigBeeClass::getLength(){
	return zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD];
}

uint16_t ZigBeeClass::getSender(){
	return AF_INCOMING_MESSAGE_SHORT_ADDRESS();
}

uint16_t ZigBeeClass::messageType(){
	return message_type;
}


bool ZigBeeClass::isOnline(){
	return networkOnline;
}
  
moduleResult_t ZigBeeClass::setTime(uint32_t clock){
	return sysSetTime(clock);
}

uint32_t ZigBeeClass::getTime(){
	if ( sysGetTime()==MODULE_SUCCESS ){
		printf("Date-Time: %02d/%02d/%04d - %02d:%02d:%02d\n\r",SYS_TIME_MONTH(),SYS_TIME_DAY(),SYS_TIME_YEAR(),SYS_TIME_HOUR(),SYS_TIME_MINUTE(),SYS_TIME_SECOND());
		return ((uint32_t) SYS_TIME_MSB())*65536+(uint16_t)SYS_TIME_LSB();
	}else{
		Serial.println("Time Read Fail");
		return 0;
	}
}

int ZigBeeClass::getRandom(){
	sysRandom();
	return SYS_RANDOM_RESULT();
}

void ZigBeeClass::displayNetworkInfo(){
	
}
void ZigBeeClass::displayDeviceInfo(){
	
}






ZigBeeClass ZigBee;