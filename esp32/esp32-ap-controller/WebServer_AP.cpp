
// WARNING!!! Make sure that you have selected Board ---> ESP32 Dev Module
//            Partition Scheme ---> Huge APP (3MB No OTA/1MB SPIFFS)
//            PSRAM ---> enabled
// 重要配置：
// #define HTTPD_DEFAULT_CONFIG()             \
//   {                                        \
//     .task_priority = tskIDLE_PRIORITY + 5, \
//     .stack_size = 4096,                    \
//     .server_port = 80,                     \
//     .ctrl_port = 32768,                    \
//     .max_open_sockets = 7,                 \
//     .max_uri_handlers = 8,                 \
//     .max_resp_headers = 8,                 \
//     .backlog_conn = 5,                     \
//     .lru_purge_enable = false,             \
//     .recv_wait_timeout = 5,                \
//     .send_wait_timeout = 5,                \
//     .global_user_ctx = NULL,               \
//     .global_user_ctx_free_fn = NULL,       \
//     .global_transport_ctx = NULL,          \
//     .global_transport_ctx_free_fn = NULL,  \
//     .open_fn = NULL,                       \
//     .close_fn = NULL,                      \
//   }

// Select camera model
// #define CAMERA_MODEL_WROVER_KIT
// #define CAMERA_MODEL_ESP_EYE
// #define CAMERA_MODEL_M5STACK_PSRAM

#include "SerialPacket.h"
#define CAMERA_MODEL_M5STACK_WIDE

// #define CAMERA_MODEL_AI_THINKER

#include "WebServer_AP.h"
#include "camera_pins.h"
#include "esp_system.h"

// #include "BLEAdvertisedDevice.h"
// BLEAdvertisedDevice _BLEAdvertisedDevice;

void startServer();
void WebServer_AP::WebServer_AP_Init(void)
{
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000; // 20000000
  config.pixel_format = PIXFORMAT_JPEG;
  // init with high specs to pre-allocate larger buffers
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  sensor_t *s = esp_camera_sensor_get();
  // drop down frame size for higher initial frame rate
  // s->set_framesize(s, FRAMESIZE_SXGA); //字节长度采样值:60000                 #9 (画质高)  1280x1024
  s->set_framesize(s, FRAMESIZE_SVGA); // 字节长度采样值:40000                   #7 (画质中)  800x600
                                       // s->set_framesize(s, FRAMESIZE_QVGA); //字节长度采样值:10000                #4 (画质低)  320x240

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 0);
  s->set_hmirror(s, 1);
#endif
  s->set_vflip(s, 0);   // 图片方向设置（上下）
  s->set_hmirror(s, 0); // 图片方向设置（左右）

  // s->set_vflip(s, 1);   //图片方向设置（上下）
  // s->set_hmirror(s, 1); //图片方向设置（左右）

  Serial.println("\r\n");

  uint64_t chipid = ESP.getEfuseMac();
  char string[10];
  sprintf(string, "%04X", (uint16_t)(chipid >> 32));
  String mac0_default = String(string);
  sprintf(string, "%08X", (uint32_t)chipid);
  String mac1_default = String(string);
  String url = ssid + mac0_default + mac1_default;
  const char *mac_default = url.c_str();

  Serial.println(":----------------------------:");
  Serial.print("wifi_name:");
  Serial.println(mac_default);
  Serial.println(":----------------------------:");
  wifi_name = mac0_default + mac1_default;

  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.mode(WIFI_AP);

  if (!WiFi.softAP(mac_default, password, 9))
  {
    //Serial2.print("{\"APCreation\": \"Failed\"}");
    Serial.println("APCreation Failed");
  }
  else
  {
    //Serial2.print("{\"APCreation\": \"Success\"}");
    Serial.println("APCreation Success");
  }

  startServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.softAPIP());
  Serial.println("' to connect");
}

// bool WebServer_AP::HandleRequest(WiFiClient client)
// {
//   if (client)
//   {
//     Serial.println("New Client.");
//     String currentLine = "";
//     while (client.connected())
//     {
//       if (client.available())
//       {
//         char c = client.read();
//         Serial.write(c);
//         if (c == '\n')
//         {

//           // if the current line is blank, you got two newline characters in a row.
//           // that's the end of the client HTTP request, so send a response:
//           if (currentLine.length() == 0)
//           {
//             // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
//             // and a content-type so the client knows what's coming, then a blank line:
//             client.println("HTTP/1.1 200 OK");
//             client.println("Content-type:application/json");
//             client.println();

//             // the content of the HTTP response follows the header:
//             client.print("\"\"");

//             // The HTTP response ends with another blank line:
//             client.println();
//             // break out of the while loop:
//             break;
//           }
//           else
//           { // if you got a newline, then clear currentLine:
//             currentLine = "";
//           }
//         }
//         else if (c != '\r')
//         {
//           currentLine += c;
//         }

//         // Check to see if the client request was "GET /ultrasonic" or "POST /cmd_vel"
//         if (currentLine.endsWith("GET /ultrasonic"))
//         {
//           Serial.println("====GET Recieved====");
//           Serial.println("/ultrasonic");
//           SerialPacket packet = SerialPacket(); // make the serial packet

//           unsigned char command[4];
//           command[0] = ULTRASONIC; // command header
//           command[1] = ARGINT; // argument type
//           command[2] = GET;    // command type
//           command[3] = 0;      // checksum

//           int build_fail = packet.Build(command, sizeof(command)); // build the packet
//           if (!build_fail)
//           {
//             Serial.println("Sending serial packet");
//             packet.Send();
//           }
//         }
//         if (currentLine.endsWith("POST /cmd_vel"))
//         {
//           Serial.println("====POST Recieved====");
//           Serial.println("/cmd_vel");

//           SerialPacket packet;

//           unsigned char commands[5];

//           String json;
//           while (client.available())
//           {
//             char character = client.read();

//             if (character == '\n') //hopefully end of document hehe
//             {
//               Serial.println(json);
//               break;
//             }

//             json += character;
//           }

//           StaticJsonDocument<200> doc;
//           DeserializationError err = deserializeJson(doc, json);

//           float linear_x = doc["linear"]["x"];
//           float angular_z = doc["angular"]["z"];

//           commands[0] = MOVE;   // command header
//           commands[1] = ARGINT; // argument type
//           commands[2] = linear_x;      // command type
//           commands[3] = angular_z;
//           commands[4] = 0; //checksum
//         }
//       }
//     }
//     // close the connection:
//     client.stop();
//     Serial.println("Client Disconnected.");
//   }
// }