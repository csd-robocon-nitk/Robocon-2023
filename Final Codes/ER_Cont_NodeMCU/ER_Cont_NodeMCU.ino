#include <PS2X_lib.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "CRN1";
const char* password = "robocon2k23";

// Pins for PS2 Contoller
#define PS2_DAT D7
#define PS2_CMD D6
#define PS2_SEL D8
#define PS2_CLK D5

PS2X ps2x;
int pwm = 100;
int deadzone = 10;
String msg;

// Defining a web server object
AsyncWebServer server(80);

// Callback function to send a string of data when a request is recieved
String sendps2() {
  ps2x.read_gamepad();

  // Read all the buttons and joystick values in the controller
  int ly = ps2x.Analog(PSS_LY);
  int lx = ps2x.Analog(PSS_LX);
  int rx = ps2x.Analog(PSS_RX);
  int mega_rst = ps2x.Button(PSB_CIRCLE);
  int PickUp = ps2x.Button(PSB_TRIANGLE);
  int PickDown = ps2x.Button(PSB_SQUARE);
  int Reload = ps2x.Button(PSB_R1);
  int Shoot = ps2x.Button(PSB_L1);
  int Angle_Up = ps2x.Button(PSB_PAD_UP);
  int Angle_Down = ps2x.Button(PSB_PAD_DOWN);
  int pull = ps2x.Button(PSB_PAD_LEFT);
  int push = ps2x.Button(PSB_PAD_RIGHT);
  int RstMega = ps2x.Button(PSB_CROSS);
  int RstNano = ps2x.Button(PSB_CIRCLE);
  int RstMPU = ps2x.Button(PSB_R2);
  float vl = ((128 - ly) * 100 / 128.0);
  float vn = ((lx - 128) * 100 / 128.0);
  float vr = ((rx - 128) * 100 / 128.0);

  int tilt, lift, power;
  if (PickUp) lift = 1;
  else if (PickDown) lift = -1;
  else lift = 0;

  if (Angle_Up) tilt = 1;
  else if (Angle_Down) tilt = -1;
  else tilt = 0;

  if (pull) power = 1;
  else if (push) power = -1;
  else power = 0;

  if (RstMega)
    msg = "rst_mega";
  else if (RstMPU)
    msg = "rst_mpu";
  else if (RstNano)
    msg = "rst_nano";
  else if (Shoot)
    msg = "sht";
  else if (Reload)
    msg = "rel";
  else
    msg = String("val") + String(" ") + String(vl) + String(" ") + String(vn) + String(" ") + String(vr) + String(" ") + String(lift) + String(" ") + String(power) + String(" ") + String(tilt);
  // Serial.println(msg); ---------> For debugging purposes

  // Send message string to client
  return msg;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Configure PS2 controller
  int error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);

  // Set up WiFi access point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Connected");
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Set up server
  server.on("/controller", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/plain", sendps2().c_str());
  });
  server.begin();
}

void loop() {
  
}
