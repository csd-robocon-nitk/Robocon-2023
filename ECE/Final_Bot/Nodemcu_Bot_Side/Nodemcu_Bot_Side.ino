/*
  Code for Nodemcu(ESP8266-07) on the robot side
  It is connected to serial port of Arduino Mega. Refer to the below code for pins
  It acts as a HTTP client
  The data is requested from http://<AP IP address of the hotspot obtained from the other nodemcu>/controller
*/
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

#include "ESP8266TimerInterrupt.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>

#define RST 12

ESP8266WiFiMulti WiFiMulti;

// NOTE: Enter your ssid and password of your mobile hotspot
const char* ssid = "CRN";
const char* password = "robocon2k23";

//Your IP address or domain name with URL path(refer to the nodemcu code on remote controller side for the IP address)
const char* serverName = "http://192.168.0.107/controller";

String skps;

float z = 0;
sensors_event_t a, g, temp;
float vel_z;
float cur_z;

float err_z;

Adafruit_MPU6050 mpu;

#define TIMER_INTERVAL_MS        500   //1000

ESP8266Timer ITimer;

void TimerHandler()
{
  vel_z = g.gyro.z - err_z;
  cur_z = vel_z*0.573;
  z = (fabs(cur_z)>0.03)?z + (cur_z):z;
}

void setup() 
{
  // Initializing serial interfaces
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RST, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
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
  
  mpu.enableSleep(false);
  mpu.enableCycle(false);
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(1000);
  Serial.println("Calibrating....Do not move mpu6050");
  mpu.getEvent(&a, &g, &temp);
  err_z = g.gyro.z;
  Serial.println("Done");
  Serial.println("");
  delay(2000);
  pinMode(LED_BUILTIN, OUTPUT);
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 20, TimerHandler))
  {
    Serial.print(F("Starting  ITimer OK, millis() = ")); Serial.println(millis());
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
}

#define CHECK_INTERVAL_MS     10000L
#define CHANGE_INTERVAL_MS    20000L

void loop() 
{
  // Retrieving data from HTTP server on the NodeMCU
  mpu.getEvent(&a, &g, &temp);
  skps = httpGETRequest(serverName);
  if(skps == "rst") 
    digitalWrite(RST, LOW);
  else 
    digitalWrite(RST, HIGH);
  
  if(skps == "rstmpu")
    z = 0;
  
  long rssi = WiFi.RSSI();
  skps = skps + " " + String(z);
  
  if (rssi <= -80)
    Serial.println("val 0.00 0.00 0.00 0.00 "+String(z));
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

  String payload = "val 0.00 0.00 0.00 0.00";

  if (httpResponseCode>0)
  {
    payload = http.getString();
  }
  http.end();

  return payload;
}

