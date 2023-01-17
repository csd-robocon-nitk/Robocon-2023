#include <PS2X_lib.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "CRN";
const char* password = "robocon2k23";

#define PS2_DAT        D7   
#define PS2_CMD        D6
#define PS2_SEL        D8 
#define PS2_CLK        D5

PS2X ps2x;
AsyncWebServer server(80);
String msg; 

String sendps2() {
  ps2x.read_gamepad();  
  int LY = ps2x.Analog(PSS_LY);
  int LX = ps2x.Analog(PSS_LX);
  int RY = ps2x.Analog(PSS_RY);
  int RX = ps2x.Analog(PSS_RX);
  int i = 0;
  float vl = (128-LY)*1.4/128.0;
  float vn = (LX-128)*1.4/128.0;
  float vr = (RX-128)*0.05/128.0;
  msg = String("val")+String(" ")+String(vl)+String(" ")+String(vn)+String(" ")+String(vr); 
  Serial.println(msg);
  return msg;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Connect the Controller");
  delay(10000);
  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
  if(error == 0){
    digitalWrite(LED_BUILTIN, HIGH);   
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  IPAddress IP = WiFi.localIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/controller", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", sendps2().c_str());
  });
  server.begin();
}

void loop(){
  
}