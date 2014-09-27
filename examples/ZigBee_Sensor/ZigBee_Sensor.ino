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
  ZigBee.setPollRate(5000);
  ZigBee.start(END_DEVICE);
}

int count=0;

void loop()
{
      Serial.print("Message Sent: ");
      Serial.println(count);
      ZigBee.addValue(count++);
      ZigBee.send(0);
      delay(2000);
}

