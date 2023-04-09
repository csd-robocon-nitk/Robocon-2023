#include <PS2X_lib.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "CRN";
const char* password = "robocon2k23";

#define PS2_DAT D7
#define PS2_CMD D6
#define PS2_SEL D8
#define PS2_CLK D5

PS2X ps2x;
int pwm = 100;
int deadzone = 10;
AsyncWebServer server(80);
String msg;

String sendps2() {
  ps2x.read_gamepad();
  int LY = ps2x.Analog(PSS_LY);
  int LX = ps2x.Analog(PSS_LX);
  int RY = ps2x.Analog(PSS_RY);
  int RX = ps2x.Analog(PSS_RX);
  int Rst = ps2x.Button(PSB_CIRCLE);
  int PickUp = ps2x.Button(PSB_TRIANGLE);
  int PickDown = ps2x.Button(PSB_SQUARE);
  int Reload = ps2x.Button(PSB_R1);
  int Shoot = ps2x.Button(PSB_L1);
  int UP = ps2x.Button(PSB_PAD_UP);
  int DOWN = ps2x.Button(PSB_PAD_DOWN);
  int LEFT = ps2x.Button(PSB_PAD_LEFT);
  int RIGHT = ps2x.Button(PSB_PAD_RIGHT);
  int RstMpu = ps2x.Button(PSB_CROSS);
  int i = 0;
  float vl = (128 - LY) * 100 / 128.0;
  float vn = (LX - 128) * 100 / 128.0;
  float vr = (RX - 128) * 100 / 128.0;

  int tilt, lift, power;
  if (PickUp) lift = 1;
  else if (PickDown) lift = -1;
  else lift = 0;
  
  if (UP) tilt = 1;
  else if (DOWN) tilt = -1;
  else tilt = 0;

  if (LEFT) power = 1;
  else if (RIGHT) power = -1;
  else power = 0;
  
  if (Rst) 
    msg = "rst";
  else if (RstMpu)
    msg = "rstmpu";
  else 
    msg = String("val") + String(" ") + String(vl) + String(" ") + String(vn) + String(" ") + String(vr) + String(" ") + String(lift) + String(" ") + String(tilt) + String(" ") + String(Reload) + String(" ") + String(power) + String(" ") + String(Shoot);
  Serial.println(msg);
  return msg;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  IPAddress IP = WiFi.localIP();
  Serial.print("Connected ");
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/controller", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", sendps2().c_str());
  });
  server.begin();
}

void loop() {
}