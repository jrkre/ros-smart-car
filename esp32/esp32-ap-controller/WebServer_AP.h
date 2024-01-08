/*
 * @Descripttion: 
 * @version: 
 * @Author: Elegoo
 * @Date: 2020-06-04 11:42:27
 * @LastEditors: Changhua
 * @LastEditTime: 2020-07-23 14:21:48
 */

#ifndef _CameraWebServer_AP_H
#define _CameraWebServer_AP_H
#include "esp_camera.h"
#include <WiFi.h>
#include "SerialPacket.h"
#include "ArduinoJson.h"

class WebServer_AP
{

public:
  void WebServer_AP_Init(void);
  String wifi_name;

  // bool HandleRequest(WiFiClient client);


private:
  // const char *ssid = "ESP32_CAM";
  // const char *password = "elegoo2020";
  char *ssid = "ELEGOO-";
  //char *password = "elegoo2020";
  char *password = "";
};

#endif
