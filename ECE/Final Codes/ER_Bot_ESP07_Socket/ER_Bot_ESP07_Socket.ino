// Some constants to setup timers
#define TIMER_INTERRUPT_DEBUG         0
#define TIMERINTERRUPT_LOGLEVEL     0

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

#include "ESP8266TimerInterrupt.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ArduinoWebsockets.h>

// Reset pins for mega and nano
#define rst_mega 13
#define rst_nano 14

// ssid and password of ER controller AP
const char* ssid = "CRN1";
const char* password = "robocon2k23";
int port = 80;

using namespace websockets;

WebsocketsClient client;

String msg;
String subnet;

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
  pinMode(rst_mega, OUTPUT);
  pinMode(rst_nano, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(rst_mega, HIGH);  
  digitalWrite(rst_nano, HIGH);

  // Connecting to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

  // Fetching IP Address
  IPAddress localIp = WiFi.localIP();
  subnet = String(localIp[0]) + "." + String(localIp[1]) + "." + String(localIp[2]) + ".1";
  Serial.println(subnet);
  
  //mpu settings
  mpu.enableSleep(false);
  mpu.enableCycle(false);
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("");
  delay(1000);

  //Calibrating MPU6050
  Serial.println("Calibrating....Do not move mpu6050");
  mpu.getEvent(&a, &g, &temp);
  err_z = g.gyro.z;
  Serial.println("Done");
  Serial.println("");
  delay(2000);
  pinMode(LED_BUILTIN, OUTPUT);

  //Enabling timer
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 20, TimerHandler))
  {
    Serial.print(F("Starting  ITimer OK, millis() = ")); Serial.println(millis());
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
  
  // Connect client to server at port 80
  bool connected = client.connect(subnet, 80, "/");

  // Callback function to be executed when client recieves data
  client.onMessage([](WebsocketsMessage message){
    msg = message.data();

    // Arduino Mega reset logic
    if(msg == "rst_mega") 
      digitalWrite(rst_mega, LOW);
    else
      digitalWrite(rst_mega, HIGH);

    // Arduino Nano reset logic
    if(msg == "rst_nano")
      digitalWrite(rst_nano, LOW);
    else 
      digitalWrite(rst_nano, HIGH);

    //Reseting MPU on command
    if(msg == "rst_mpu")
      z = 0;
    
    // Wifi strength check
    long rssi = WiFi.RSSI();
    msg = msg + " " + String(z);
    if (rssi <= -80)
      Serial.println("val 0.00 0.00 0.00 0 0 0 0 0 0"+String(z));
    else
      Serial.println(msg);
  });
}

#define CHECK_INTERVAL_MS     10000L
#define CHANGE_INTERVAL_MS    20000L

void loop() 
{
  // Keep checking for new message from server
  if(client.available()) {
    client.poll();
  }
  mpu.getEvent(&a, &g, &temp);
}