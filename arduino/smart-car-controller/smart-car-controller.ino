#include <avr/wdt.h>
#include "DeviceDriverSet.h"
#include "SerialPacket.h"
#include "SerialDriver.h"

DeviceDriverSet_RBGLED AppRBG_LED;
DeviceDriverSet_ULTRASONIC AppULTRASONIC;
SerialDriver serialDriver;

#define DEBUG 0



void setup()
{
  Serial.begin(BAUD_RATE);
  // Serial.println("serial init");
  wdt_enable(WDTO_4S);

  serialDriver.SerialDriver_Init();
  
}

void loop()
{

  //serialDriver.DriveForward(10);
  // serialDriver.UpdateSensors();
  // serialDriver.ReadSerialCommand();
  // serialDriver.RunCommand();
  serialDriver.UpdateSensors();
  serialDriver.advance_state();

  wdt_reset();

  delay(20);
}
