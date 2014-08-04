#include "Energia.h"
#include "SPI.h"
#include <ZigBee.h>
#include <stdint.h>



void setup()
{
  Serial.begin(115200);
  ZigBee.begin();
  ZigBee.setRegion(NORTH_AMERICA);
  ZigBee.setSecurityMode(SECURITY_MODE_PRECONFIGURED_KEYS);
  ZigBee.setSecurityKeys("1234567812345678");
  ZigBee.start(COORDINATOR);
  ZigBee.setTime(454352024);
}

void loop()
{
  ZigBee.getTime();
  if(ZigBee.checkMessage()){
    Serial.print("Message Received: ");
    Serial.println(ZigBee.getValue());
  }
  delay(2000);

}
