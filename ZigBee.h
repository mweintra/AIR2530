#ifndef zigbee_h
#define zigbee_h

#include <stdint.h>

class ZigBeeClass {

private:


public:

  ZigBeeClass();
  // setup pin configurations
    void setMRSTpin(uint8_t pin);
	void setMRDYpin(uint8_t pin);
	void setSRDYpin(uint8_t pin);
	void setSPImodule(uint8_t module);
  void init(); // Default
  
  
  void begin();
  void reset();
  void getMac();
  void setType();  
};

extern ZigBeeClass ZigBee;

#endif
