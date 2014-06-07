

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
	#if defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____) 
		MRSTpin=PE_0;
		MRDYpin=PA_5;
		SRDYpin=PA_7;
		SPImodule=2;
	#endif
	#if defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) 
		MRSTpin=PH_2;
		MRDYpin=PC_7;
		SRDYpin=PB_3;
		SPImodule=2;	
	#endif
}

/************************** ADD MESSAGE HEADERS ********************************/
uint8_t ZigBeeClass::getHeaderLength(uint8_t headers){
	uint8_t headerCount=1;
	if (headers & SOURCE_TIME_HEADER){
		headerCount+=SOURCE_TIME_LENGTH;
	}
	if (headers &	SOURCE_MAC_HEADER){
		headerCount+=SOURCE_MAC_LENGTH;
	}
	if (headers & PARENT_MAC_HEADER){
		headerCount+=PARENT_MAC_LENGTH;	
	}
	if (headers & SOURCE_LQI_HEADER){
		headerCount+=SOURCE_LQI_LENGTH;
	}
	if (headers & SOURCE_LATENCY_HEADER){
		headerCount+=SOURCE_LATENCY_LENGTH;
	}
	if (headers & DEVICE_TYPE_HEADER){
		headerCount+=DEVICE_TYPE_LENGTH;
	}
	return headerCount;
}
void ZigBeeClass::addHeaders(){
	uint8_t headerCount=0;
	int i;
	transmitBufferIndex=0;
	addByte(messageHeader);
	if (messageHeader & SOURCE_TIME_HEADER){
		uint32_t thisTime=getTime();
		if ( !(messageHeader & TIME_SYNC_MESSAGE) ){
			addByte(zmBuf[SYS_TIME_UTC_FIELD]);
			addByte(zmBuf[SYS_TIME_UTC_FIELD+1]);
			addByte(zmBuf[SYS_TIME_UTC_FIELD+2]);
			addByte(zmBuf[SYS_TIME_UTC_FIELD+3]);
		}else{
			uint32_t offset = thisTime-messageReceivedTime;
			addByte(LSB(offset));
			addByte((offset & 0xFF00) >> 8);
			addByte((offset & 0xFF0000) >> 16);
			addByte(offset >> 24);
		}
	}
	if (messageHeader &	SOURCE_MAC_HEADER){
		zbGetDeviceInfo(DIP_PARENT_MAC_ADDRESS);
		for (i = SRSP_DIP_VALUE_FIELD; i<=SRSP_DIP_VALUE_FIELD+7; i++)
			addByte(zmBuf[i]);
	}
	if (messageHeader & PARENT_MAC_HEADER){
		zbGetDeviceInfo(DIP_MAC_ADDRESS);
		for (i = SRSP_DIP_VALUE_FIELD; i<=SRSP_DIP_VALUE_FIELD+7; i++)
			addByte(zmBuf[i]);		
	}
	if (messageHeader & SOURCE_LQI_HEADER){
		if(messageHeader & FORWARDED_MESSAGE)
			addByte(getSourceLQI());
		else
			addByte(getLQI());
	}
	if (messageHeader & SOURCE_LATENCY_HEADER){
		addByte(duration);
	}
	if (messageHeader & DEVICE_TYPE_HEADER){
		addByte(moduleConfiguration.deviceType);
	}
}
void ZigBeeClass::printHeaders(){

	printf("Message Headers: %02X\n\r",messageReceivedHeader);
	if (messageReceivedHeader & SOURCE_TIME_HEADER){
		Serial.print("Source Time: ");
		Serial.println(messageReceivedTime);

	}
	if (messageReceivedHeader &	SOURCE_MAC_HEADER){
		printf("Device MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n\r",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],mac[6],mac[7]);
	}
	if (messageReceivedHeader & PARENT_MAC_HEADER){
		printf("Parent MAC: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n\r",parent[0],parent[1],parent[2],parent[3],parent[4],parent[5],parent[6],parent[7]);
	}
	if (messageReceivedHeader & SOURCE_LQI_HEADER){
		printf("Source LQI: %02X\n\r",messageReceivedLQI);
	}
	if (messageReceivedHeader & SOURCE_LATENCY_HEADER){
		printf("Source latency: %02X\n\r",messageReceivedLatency);
	}
	if (messageReceivedHeader & DEVICE_TYPE_HEADER){
		printf("Source Device Type: %02X\n\r",messageReceivedDevice);
	}	
	Serial.println();
}

/******************** SET HARDWARE CONFIGURATIONS ****************************/
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


/********************************** MODULE METHODS ********************************/
void ZigBeeClass::begin() {
#ifdef UART_DEBUG
	displayVersion();
	displaySysResetInd();
#endif
	halInit();
    moduleInit();
	moduleReset();
	networkOnline=false;
	messageHeader = (SOURCE_TIME_HEADER | SOURCE_MAC_HEADER | SOURCE_LATENCY_HEADER | DEVICE_TYPE_HEADER);
	receiveBufferIndex=0;
	transmitBufferIndex=getHeaderLength(messageHeader);	
}
moduleResult_t ZigBeeClass::start(uint8_t deviceType){
		afSetAckMode(AF_MAC_ACK);
	/*if(deviceType==END_DEVICE)
		afSetAckMode(AF_MAC_ACK);
	else
		afSetAckMode(AF_APS_ACK);
		*/
	transmitBufferIndex=getHeaderLength(messageHeader);
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
	uint32_t startTime;
	uint8_t size=transmitBufferIndex;
	if(networkOnline){
		addHeaders();
		startTime=millis();
		if (result=afSendData(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, shortAddress, INFO_MESSAGE_CLUSTER, messageBuffer, size)!=MODULE_SUCCESS){
			if(moduleConfiguration.deviceType==END_DEVICE)
				networkOnline=false;
		}else{
			duration=(uint8_t)(millis()-startTime);
		}
		transmitBufferIndex=getHeaderLength(messageHeader);
	}
	return result;
}
moduleResult_t ZigBeeClass::send(uint16_t shortAddress, char* message,uint16_t messageSize ){
	uint32_t startTime;
	if(networkOnline){
		addHeaders();
		startTime=millis();		
		if (result=afSendData(DEFAULT_ENDPOINT, DEFAULT_ENDPOINT, shortAddress, INFO_MESSAGE_CLUSTER, (uint8_t*)message, messageSize)!=MODULE_SUCCESS){
			networkOnline=false;
		}else{
			duration=(uint8_t)(millis()-startTime);
		}
		transmitBufferIndex=getHeaderLength(messageHeader);
	}
	return result;
}



/********************** SET NETWORK CONFIGURATIONS ****************************/
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

bool ZigBeeClass::checkMessage(){
	if(moduleHasMessageWaiting()){
		uint32_t thisTime=getTime();
		getMessage();
		//THIS IS WHERE THE HEADER FIELD IS
		if (zmBuf[SRSP_LENGTH_FIELD] > 0)
		{
			// Load the ZM parameters
			message_type=(CONVERT_TO_INT(zmBuf[SRSP_CMD_LSB_FIELD], zmBuf[SRSP_CMD_MSB_FIELD]));
			incomingMessageLQI=zmBuf[AF_INCOMING_MESSAGE_LQI_FIELD];
			messageReceivedLength=zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD];
			messageReceivedSource=AF_INCOMING_MESSAGE_SHORT_ADDRESS();
			
			// Load the Message Headers
			uint16_t bufferCounter=AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD;
			messageReceivedHeader=zmBuf[bufferCounter++];
			if(messageReceivedHeader & SOURCE_TIME_HEADER){
				messageReceivedTime=(uint32_t)zmBuf[bufferCounter++];	
				messageReceivedTime+=((uint32_t)zmBuf[bufferCounter++])*256;
				messageReceivedTime+=((uint32_t)zmBuf[bufferCounter++])*65536;
				messageReceivedTime+=((uint32_t)zmBuf[bufferCounter++])*16777216;
				
				incomingMessageTime=0;
				
				incomingMessageTime+=(uint32_t)zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD];	
				incomingMessageTime+=((uint32_t)zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD+1])*256;
				incomingMessageTime+=((uint32_t)zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD+2])*65536;
				incomingMessageTime+=((uint32_t)zmBuf[AF_INCOMING_MESSAGE_TIMESTAMP_FIELD+3])*16777216;


			}
			if (messageReceivedHeader &	SOURCE_MAC_HEADER){
				for(int i=7;i>=0;i--)
					mac[i]=zmBuf[bufferCounter++];
			}
			if (messageReceivedHeader & PARENT_MAC_HEADER){
				for(int i=7;i>=0;i--)
					parent[i]=zmBuf[bufferCounter++];
			}
			if (messageReceivedHeader & SOURCE_LQI_HEADER){
				messageReceivedLQI=zmBuf[bufferCounter++];
			}
			if (messageReceivedHeader & SOURCE_LATENCY_HEADER){
				messageReceivedLatency=zmBuf[bufferCounter++];
			}
			if (messageReceivedHeader & DEVICE_TYPE_HEADER){
				messageReceivedDevice=zmBuf[bufferCounter++];
			}

			// load the message buffer			
			for ( int i=getHeaderLength(messageReceivedHeader);i<messageReceivedLength;i++){
				messageBuffer[i]=zmBuf[AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD+i];
			}

			// Synchronize time with parent node
			if(messageReceivedHeader & SOURCE_TIME_HEADER){
				if(messageReceivedHeader & TIME_SYNC_MESSAGE){
					long int nowTime=getTime();
					nowTime+= (long int) messageReceivedTime;
					Serial.println("Time Sync");
					Serial.println(messageReceivedTime);
					Serial.println(nowTime);
					setTime(nowTime);
				}
				if(moduleConfiguration.deviceType!= END_DEVICE && thisTime!=messageReceivedTime){
					printf("Time syncing child: %04x, difference: ",messageReceivedSource);
						Serial.println((long int)thisTime-(long int)messageReceivedTime);
						Serial.println(thisTime);
						Serial.println(messageReceivedTime);
					receiveBufferIndex=getHeaderLength(messageHeader);
					messageHeader += TIME_SYNC_MESSAGE;
					if(send(messageReceivedSource)!=MODULE_SUCCESS){
						Serial.print("Transmission Failed!");
					}
					messageHeader -= TIME_SYNC_MESSAGE;
				}
				
			}
		receiveBufferIndex=getHeaderLength(messageReceivedHeader);	
		transmitBufferIndex=getHeaderLength(messageHeader);	
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

/************** ADD VALUES TO MESSAGE BUFFER *********************************/
void ZigBeeClass::addByte(uint8_t data){
	messageBuffer[transmitBufferIndex++]=data;
}
void ZigBeeClass::addValue(uint16_t data){
	messageBuffer[transmitBufferIndex]=MSB(data);
	messageBuffer[transmitBufferIndex+1]=LSB(data);
	transmitBufferIndex+=2;
}
void ZigBeeClass::addKVP(uint16_t key, uint16_t value){
	messageBuffer[transmitBufferIndex]=MSB(key);
	messageBuffer[transmitBufferIndex+1]=LSB(key);
	messageBuffer[transmitBufferIndex+2]=MSB(value);
	messageBuffer[transmitBufferIndex+3]=LSB(value);	
	transmitBufferIndex+=4;
}

/*************** GET VALUES FROM MESSAGE BUFFER ******************************/
uint8_t ZigBeeClass::getByte(){
	return messageBuffer[receiveBufferIndex++];
}
uint16_t ZigBeeClass::getValue(){
	receiveBufferIndex+=2;
	return (CONVERT_TO_INT(messageBuffer[receiveBufferIndex+1],messageBuffer[receiveBufferIndex]));

}
uint16_t ZigBeeClass::getKVP(uint16_t key){
	for (int i=getHeaderLength(messageReceivedHeader);i<messageReceivedLength;i+=4){
		if (key ==(CONVERT_TO_INT(messageBuffer[i+1],messageBuffer[i]))){
			return (CONVERT_TO_INT(messageBuffer[i+3],messageBuffer[i+2]));
		}
	}
}
uint8_t ZigBeeClass::getLQI(){
	return incomingMessageLQI;
}
uint8_t ZigBeeClass::getLength(){
	return messageReceivedLength;
}
uint16_t ZigBeeClass::getSource(){
	return messageReceivedSource;
}
uint8_t ZigBeeClass::getSourceLQI(){
	return messageReceivedLQI;
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
		//printf("Date-Time: %02d/%02d/%04d - %02d:%02d:%02d\n\r",SYS_TIME_MONTH(),SYS_TIME_DAY(),SYS_TIME_YEAR(),SYS_TIME_HOUR(),SYS_TIME_MINUTE(),SYS_TIME_SECOND());
		return ((uint32_t) SYS_TIME_MSB())*65536+(uint16_t)SYS_TIME_LSB();
	}else{
		return 0;
	}
}

void ZigBeeClass::printTime(){
	if ( sysGetTime()==MODULE_SUCCESS ){
		printf("Date-Time: %02d/%02d/%04d - %02d:%02d:%02d\n\r",SYS_TIME_MONTH(),SYS_TIME_DAY(),SYS_TIME_YEAR(),SYS_TIME_HOUR(),SYS_TIME_MINUTE(),SYS_TIME_SECOND());
	}else{
		Serial.println("Get Time Fail");
	}
}

int ZigBeeClass::getRandom(){
	sysRandom();
	return SYS_RANDOM_RESULT();
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



uint8_t ZigBeeClass::getDuration(){
	return duration;
}


void ZigBeeClass::displayNetworkInfo(){
	
}
void ZigBeeClass::displayDeviceInfo(){
	
}






ZigBeeClass ZigBee;