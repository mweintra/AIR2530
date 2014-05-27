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
#define MAX_DATA_SIZE			66

class ZigBeeClass {

private:
	bool networkOnline;
	uint8_t sendBuffer[MAX_DATA_SIZE];
	uint8_t sendBufferIndex;
	uint8_t receiveBufferIndex;
	uint16_t message_type;
public:

  ZigBeeClass();

  // set up pin & SPI configurations
  void setMRSTpin(uint8_t pin);
  void setMRDYpin(uint8_t pin);
  void setSRDYpin(uint8_t pin);
  void setSPImodule(uint8_t module);
  // initiate module
  void begin();
	
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
  // send data
  moduleResult_t send(uint16_t shortAddress);
  moduleResult_t send(uint16_t shortAddress,char* message,uint16_t messageSize);
  
  // receive and parse data
  bool hasMessage();
  uint16_t messageType();
  void printMessage();
  moduleResult_t receive();
  void put(uint8_t data);
  void addValue(uint16_t data);
  void addKVP(uint16_t key,uint16_t value);
  uint8_t get(uint8_t index);
  uint16_t getValue();
  uint16_t getKVP(uint16_t key);  
  uint8_t getLQI();
  uint8_t getLength();
  uint16_t getSender();
  
  moduleResult_t setTime(uint32_t clock);
  uint32_t getTime();
  int getRandom();
  
void displayNetworkInfo();
void displayDeviceInfo();

  void reset();
  void getMac();

  
};

extern ZigBeeClass ZigBee;

#endif
