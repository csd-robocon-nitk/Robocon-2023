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

#define RST D2 

ESP8266WiFiMulti WiFiMulti;

// NOTE: Enter your ssid and password of your mobile hotspot
const char* ssid = "CRN";
const char* password = "robocon2k23";

//Your IP address or domain name with URL path(refer to the nodemcu code on remote controller side for the IP address)
const char* serverName = "http://192.168.0.107/controller";

String skps;

void setup() 
{
  // Initializing serial interfaces
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RST, HIGH);  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Connecting to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Connected to WiFi");
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() 
{
  // Retrieving data from HTTP server on the NodeMCU
  skps = httpGETRequest(serverName);
  if(skps == "rst") digitalWrite(RST, LOW);
  else digitalWrite(RST, HIGH);
  
  long rssi = WiFi.RSSI();
  
  if (rssi <= 80)
    Serial.println("val 0.00 0.00 0.00 0.00");
  else
    Serial.println(skps);
  
}

String httpGETRequest(const char* serverName)
{
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  String payload = "val 0.00 0.00 0.00";

  if (httpResponseCode>0)
  {
//    Serial.print("HTTP Response code: ");
//    Serial.println(httpResponseCode);
    payload = http.getString();
  }

  else
  {
//    Serial.print("Error code: ");
//    Serial.println(httpResponseCode);
  }
  http.end();

  return payload;
}

