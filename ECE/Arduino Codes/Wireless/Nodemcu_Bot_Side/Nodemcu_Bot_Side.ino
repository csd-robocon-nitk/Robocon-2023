/*
  Code for Nodemcu(ESP8266) on the robot side
  It is connected to serial port of Arduino Mega. Refer to the below code for pins
  It acts as a HTTP client
  The data is requested from http://<AP IP address of the hotspot obtained from the other nodemcu>/controller
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

// NOTE: Enter your ssid and password of your mobile hotspot
const char* ssid = "CRN";
const char* password = "roboconnitk";

//Your IP address or domain name with URL path(refer to the nodemcu code on remote controller side for the IP address)
const char* serverName = "http://192.168.75.137/controller";

String skps;

void setup() {
  // Initializing serial interfaces
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // Connecting to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected to WiFi");
}

void loop() {
  // Retrieving data from HTTP server on the NodeMCU
  skps = httpGETRequest(serverName);
  // Send the obtained data to serial port connected to the Mega on the robot
  Serial.println(skps);
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
//    Serial.print("HTTP Response code: ");
//    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
//    Serial.print("Error code: ");
//    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
