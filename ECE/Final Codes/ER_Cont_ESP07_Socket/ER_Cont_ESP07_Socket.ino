#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

#define LF 0x0A

// Setting up wifi ssid and password
const char* ssid = "CRN2";
const char* password = "robocon2k23";

int isConnected = 0;

using namespace websockets;

// For websocket
WebsocketsServer server;
WebsocketsClient client;

String msg;

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

  // Put all data into port 80
  server.listen(80);
}

void loop() {
  if(isConnected == 0) {
    // Allow only one client to connect to this controller
    client = server.accept();
    isConnected = 1;
  }
  if(client.available()) {
    // Read message from Serial
    if (Serial.available() > 0)
      msg = Serial.readStringUntil('\n');
    
    // Send message to client
    client.send(msg);
  }
  else {
    isConnected = 0;
  }
}