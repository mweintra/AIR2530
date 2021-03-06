#ifndef zigbee_h
#define zigbee_h

#include <stdint.h>
#include "utility/module_errors.h"
#include "utility/module.h"
#include "utility/utilities.h"
#include "utility/module_commands.h"

#define NORTH_AMERICA         	0x0000
#define EUROPE                	0x0001
#define INFO_MESSAGE_CLUSTER  	0x07
#define MAX_DATA_SIZE			64

#define SOURCE_TIME_HEADER		0x01
#define	SOURCE_MAC_HEADER		0x02
#define PARENT_MAC_HEADER		0x04
#define SOURCE_LQI_HEADER		0x08
#define SOURCE_LATENCY_HEADER	0x10
#define DEVICE_TYPE_HEADER		0x20


// Basic sensor-gateway messaging schemes
#define FORWARDED_MESSAGE			0x40
#define TIME_SYNC_MESSAGE 			0x80


#define SOURCE_TIME_LENGTH		4
#define	SOURCE_MAC_LENGTH		8
#define PARENT_MAC_LENGTH		8
#define SOURCE_LQI_LENGTH		1
#define SOURCE_LATENCY_LENGTH	1
#define DEVICE_TYPE_LENGTH		1

#define MESSAGE_HEADERS			AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD

class ZigBeeClass {

private:
	bool networkOnline;
	uint8_t messageBuffer[MAX_DATA_SIZE];
	uint8_t transmitBufferIndex;
	uint8_t receiveBufferIndex;

	uint16_t message_type;
	uint8_t getHeaderLength(uint8_t headers);
	uint8_t duration;

	uint16_t messageReceivedSource;
	uint8_t messageReceivedHeader;
	uint32_t messageReceivedTime;
	
	uint8_t incomingMessageLQI;
	uint32_t incomingMessageTime;
	uint8_t mac[8];
	uint8_t parent[8];
	uint8_t messageReceivedLQI;
	uint8_t messageReceivedLatency;
	uint8_t messageReceivedDevice;
	
	bool timeSync;
	
	uint8_t messageReceivedLength;
	
	uint8_t getByte();
	void addByte(uint8_t data);
	void addHeaders();
	
	
public:
	uint8_t messageHeader;
	//	uint8_t receiveBufferIndex;
  ZigBeeClass();
	// set up pin & SPI configurations
	void setMRSTpin(uint8_t pin);
	void setMRDYpin(uint8_t pin);
	void setSRDYpin(uint8_t pin);
	void setSPImodule(uint8_t module);
	// initiate module
	void begin();
	
	//void enableTime();
	//void enableLQI();

	// set up module configurations
	void setRegion(uint16_t region);  
	void setChannelMask(uint32_t channelMask);  
	void setPanID(uint16_t panId);
	void setPollRate(uint16_t endDevicePollRate);
	void setStartupOptions(uint8_t startupOptions);
	void setSecurityMode(uint8_t securityMode);
	void setSecurityKeys(char* securityKey);

	// start module as specified device
	moduleResult_t start(uint8_t deviceType);
	bool isOnline();
  
  
	// format message data for sending

	void addValue(uint16_t data);
	void addKVP(uint16_t key,uint16_t value);
	moduleResult_t send(uint16_t shortAddress);
	moduleResult_t send(uint16_t shortAddress,char* message,uint16_t messageSize);
  
	// read message data

	uint16_t getValue();
	uint16_t getKVP(uint16_t key);  
	bool checkMessage();
	uint16_t messageType();
	void printMessage();
	void printHeaders();
	moduleResult_t receive();

	//read message parameters
	uint8_t getLQI();
	uint8_t getSourceLQI();
	uint8_t getLength();
	uint16_t getSource();
	uint32_t sourceMac();
	int getRandom();

	void getMac();
	
	void enableTimeSync();
	void disableTimeSync();
	
	uint8_t getDuration();
	uint32_t messageSourceTime();
	moduleResult_t setTime(uint32_t clock);
	uint32_t getTime();
	void printTime();
  
	//void NVwrite(uint8_t address, uint8_t data);
	//uint8_t NVread(uint8_address);
  
  
  	void reset();
  
  
  
	void displayNetworkInfo();
	void displayDeviceInfo();
	
};

extern ZigBeeClass ZigBee;

#endif
