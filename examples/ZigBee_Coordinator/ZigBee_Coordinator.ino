#include "Energia.h"
#include "SPI.h"
#include <SHT2x.h>
#include <ZigBee.h>


#include <stdint.h>



void setup()
{
  Serial.begin(9600);
  ZigBee.begin();
  ZigBee.setRegion(NORTH_AMERICA);
  ZigBee.setSecurityMode(SECURITY_MODE_PRECONFIGURED_KEYS);
  ZigBee.setSecurityKeys("-APSNYC SECURITY");
  ZigBee.start(COORDINATOR);
  ZigBee.setTime(454352024);
}

void loop()
{
  ZigBee.getTime();
  if(ZigBee.hasMessage()){
    Serial.println("Message Received!");
    ZigBee.receive();
  }
  delay(2000);

}



