#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

#define LF 0x0A

// Setting up wifi ssid and password
const char* ssid = "CRN1";
const char* password = "robocon2k23";

AsyncWebServer server(80);
String msg;
int idx;

// Function to take data from serial and send it through wifi
String send_data() {
  // Read a string from serial
  if (Serial.available() > 0)
    msg = Serial.readStringUntil('\n');
  
  return msg;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // Creating an access point with specified ssid and password
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Connected");
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Creating server for controller
  server.on("/controller", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", send_data().c_str());
  });
  server.begin();
}

void loop() {
}