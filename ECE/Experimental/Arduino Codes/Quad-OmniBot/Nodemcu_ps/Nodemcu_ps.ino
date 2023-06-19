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
int pwm = 100;
int deadzone = 10;
AsyncWebServer server(80);
String msg; 

String sendps2() {
  float p1 = 0,p2 = 0;
  ps2x.read_gamepad();  
  int LY = ps2x.Analog(PSS_LY);endps2
  int LX = ps2x.Analog(PSS_LX);
  int RY = ps2x.Analog(PSS_RY);
  int RX = ps2x.Analog(PSS_RX);
  int i = 0;
  if (abs(128-LY)>deadzone || abs(128-LX)>deadzone){
    p1 = (128-LY)/128.0;       // CONTROL VARS FOR WHEELS 1,3
    p2 = (128-LX)/128.0;       // CONTROL VARS FOR WHEELS 2,4
    msg = String("val")+String(" ")+String(round(p1*pwm))+String(" ")+String(round(p2*pwm))+String(" ")+String(round(p1*pwm))+String(" ")+String(round(p2*pwm)); 
  }
  if(abs(128-RX)>deadzone || abs(128-RY)>deadzone) {
    float vel = (128-RX)/128.0; 
    p1 = (vel+p1)/2.0;    
    p2 = (p2-vel)/2.0;  
    msg = String("val")+String(" ")+String(round(p2*pwm))+String(" ")+String(round(p1*pwm))+String(" ")+String(round(p1*pwm))+String(" ")+String(round(p2*pwm));
  }
  if(!(abs(128-LY)>deadzone || abs(128-LX)>deadzone||abs(128-RX)>deadzone || abs(128-RY)>deadzone)){
    p1 = 0;
    p2 = 0;
    msg = String("val")+String(" ")+String(round(p2*pwm))+String(" ")+String(round(p1*pwm))+String(" ")+String(round(p1*pwm))+String(" ")+String(round(p2*pwm)); 
  }
  Serial.println(msg);
  return msg;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
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