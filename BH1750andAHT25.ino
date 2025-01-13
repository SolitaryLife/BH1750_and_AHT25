#include <Wire.h>
#include <WiFi.h>
#include <BH1750.h>
#include <ArduinoJson.h>

StaticJsonDocument<200> jsonDoc;

#define IO21 21 // SDA = GPIO21
#define IO22 22 // SCL = GPIO22

// Config WIFI
const char* _ssid = "xxxxx";      
const char* _password = "xxxxx";

// Config Node-RED (TCP/IP)
const char* _serverIP = "xxx.xxx.xxx.xxx";  
const int _serverPort = 0000;

WiFiClient espClient;

// Config BH1750
BH1750 lightMeter;
bool _light = false;
float _lux = 0.00;

#include "Func.h"

void setup() {
  Serial.begin(115200);

  WiFi.begin(_ssid, _password);

  Wire.begin(IO21, IO22);

  _light = initializeBH1750(5, 3000);  // Five attempts are given, with a delay of 3 seconds.

}

void loop() {
  
  bool connectWIFI = checkWiFiConnection();
  bool connectNode = espClient.connected();

 if (connectWIFI && !connectNode) {
        Serial.println("Disconnected from server. Reconnecting...");
        reconnectTCP();
         return;
    }

  // BH1750
  _lux = _light ? lightMeter.readLightLevel() : NAN;

  // Crate JSON and Show in Serial Monitor
  String jsonOutput = generateJson();
  Serial.println(jsonOutput);
  espClient.println(jsonOutput);

  delay(2000);
}
