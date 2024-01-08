/*
 * @Descripttion: 
 * @version: 
 * @Author: Elegoo
 * @Date: 2020-06-04 11:42:27
 * @LastEditors: Changhua
 * @LastEditTime: 2020-09-07 09:40:03
 */
//#include <EEPROM.h>
#include "WebServer_AP.h"
#include <WiFi.h>
#include "esp_camera.h"
//#include <avr/wdt.h>
#include "SerialDriver.h"


WiFiServer server(100);

#define SSID "ESP32_CAM"
#define PASSWORD "robotics11"


#define RXD2 33
#define TXD2 4
WebServer_AP WebServerAP;


SerialDriver serialDriver;

void setup()
{
    Serial.begin(31250);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
    //http://192.168.4.1/control?var=framesize&val=3
    //http://192.168.4.1/Test?var=

    //Uncomment these
    WebServerAP.WebServer_AP_Init();

    server.begin();
    delay(500);
    // while (Serial.read() >= 0)
    // {
    //   /*清空串口缓存...*/
    // }
    // while (Serial2.read() >= 0)
    // {
    //   /*清空串口缓存...*/
    // }
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    Serial.println("Elegoo-2020...");
    //Serial2.print("{Factory}");

    //wdt_enable(WD_TO4S);
}


void loop()
{
    
    //WiFiClient client = server.available();

    
    //serialDriver.AdvanceState();
    
    delay(20);

    //wdt_reset();

    // if(client && WebServerAP.HandleRequest(client))
    // {
    //     client.stop();
    //     Serial.println("Client Disconnected");
    // }
}

/*
C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32/tools/esptool/esptool.exe --chip esp32 --port COM6 --baud 460800 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 
0xe000 C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32/tools/partitions/boot_app0.bin 
0x1000 C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32/tools/sdk/bin/bootloader_qio_80m.bin 
0x10000 C:\Users\Faynman\Documents\Arduino\Hex/CameraWebServer_AP_20200608xxx.ino.bin 
0x8000 C:\Users\Faynman\Documents\Arduino\Hex/CameraWebServer_AP_20200608xxx.ino.partitions.bin 

flash:path
C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32\tools\partitions\boot_app0.bin
C:\Program Files (x86)\Arduino\hardware\espressif\arduino-esp32\tools\sdk\bin\bootloader_dio_40m.bin
C:\Users\Faynman\Documents\Arduino\Hex\CameraWebServer_AP_20200608xxx.ino.partitions.bin
*/
//esptool.py-- port / dev / ttyUSB0-- baub 261216 write_flash-- flash_size = detect 0 GetChipID.ino.esp32.bin
