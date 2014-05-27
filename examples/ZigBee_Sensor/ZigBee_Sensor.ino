#include "Energia.h"
#include "SPI.h"
#include <SHT2x.h>
#include <ZigBee.h>
#include <System.h>
#include <msp430.h>

#include <stdint.h>

void setup()
{
  pinMode(P1_5,OUTPUT);
  digitalWrite(P1_5,HIGH);
  Serial.begin(9600);
  SHT2x.begin();
  ZigBee.begin();
  ZigBee.setRegion(NORTH_AMERICA);
  ZigBee.setSecurityMode(SECURITY_MODE_PRECONFIGURED_KEYS);
  ZigBee.setSecurityKeys("-APSNYC SECURITY");
  ZigBee.setPollRate(5000);
  ZigBee.setTime(454352024);
}

void loop()
{
  if(!ZigBee.isOnline()){
    //ZigBee.start(END_DEVICE);
  }
  SHT2x.softReset();
  int temp, humi, dew, battery;
  temp=SHT2x.readT();
  humi=SHT2x.readRH();
  dew=SHT2x.readD(temp,humi);
  battery=getVCC();
  ZigBee.getTime();
  printf("Battery: %d.%02dV\n\r",battery/100,battery%100);
  printf("Temperature: %02d.%02d\260C\n\r",temp/100,temp%100);
  printf("Humidity:    %02d.%02d%RH\n\r",humi/100,humi%100);
  printf("Dew Point:   %02d.%02d\260C\n\r",dew/100,dew%100);
  printf("Random:      %u\n\r",ZigBee.getRandom());  
  char message[]="Hello!";
  ZigBee.send(0,message,sizeof(message));
  Serial.println("going to sleep");
  System.sleep(2000);
}

// returns VCC in millivolts
uint16_t getVCC() {
  // start with the 1.5V internal reference
  analogReference(INTERNAL1V5);
  delay(1);
  int data = analogRead(A11);
  Serial.println("VCC: ");
  Serial.println(data);
  // if overflow, VCC is > 3V, switch to the 2.5V reference
  if (data==0x3ff) {
    analogReference(INTERNAL2V5);
    delay(1);
    data = (uint16_t)map(analogRead(A11), 0, 1023, 0, 5000);
  } 
  else {
    data = (uint16_t)map(data, 0, 1023, 0, 3000);
  }
  return data-1000;  
}




