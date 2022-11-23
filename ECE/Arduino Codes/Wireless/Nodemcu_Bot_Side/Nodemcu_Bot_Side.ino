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
#define LF 0x0A 

char msg_str[100]; 
char str_buff[7];
int idx;
String str;
int dir1 = 2, dir2 = 4, dir3 = 7, dir4 = 8;
int pwm1 = 3, pwm2 = 5, pwm3 = 6, pwm4 = 9;

ESP8266WiFiMulti WiFiMulti;

// NOTE: Enter your ssid and password of your mobile hotspot
const char* ssid = "NITK-NET";
const char* password = "2K16NITK";

//Your IP address or domain name with URL path(refer to the nodemcu code on remote controller side for the IP address)
const char* serverName = "http://10.53.28.147/controller";

String skps;

void setup() {
  // Initializing serial interfaces
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
//  pinMode(dir1,OUTPUT);
//  pinMode(dir2, OUTPUT);
//  pinMode(dir3, OUTPUT);
//  pinMode(dir4, OUTPUT);
//  pinMode(pwm1, OUTPUT);
//  pinMode(pwm2, OUTPUT);
//  pinMode(pwm3, OUTPUT);
//  pinMode(pwm4, OUTPUT);
//  analogWrite(pwm1, 0);
//  analogWrite(pwm2, 0);
//  analogWrite(pwm3, 0);
//  analogWrite(pwm4, 0);
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
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // Retrieving data from HTTP server on the NodeMCU
  skps = httpGETRequest(serverName);
  Serial.println(skps);
  skps.toCharArray(msg_str,100);
   int i = 4;
   int j = 0;
   int var =1;
   float lx =0, ly=0, rx=0, ry=0;
   if(msg_str[0]=='v'){
     while(var<=4){
       if(msg_str[i]==' ' || msg_str[i]==0){   
         str_buff[j]=0;
         switch(var){
           case 1:
             lx = atof(str_buff);
             break;
           case 2:
             ly = atof(str_buff);
             break;
           case 3:
             rx = atof(str_buff);
             break;
           case 4:
             ry = atof(str_buff);
             break;
         }
         var++;
         j = -1;
       }
       else{
         str_buff[j]=msg_str[i];        
       }
       i++;
       j++;
     }  
   }
   Serial.print("motor1:");
   Serial.print(lx);
   Serial.print(" motor2:");
   Serial.print(ly);
   Serial.print(" motor3:");
   Serial.print(rx);
   Serial.print(" motor4:");
   Serial.println(ry);
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

// void move(int p1,int p2,int p3,int p4)
// {
//   analogWrite(pwm1, abs(p1));
//   analogWrite(pwm2, abs(p2));
//   analogWrite(pwm3, abs(p3));
//   analogWrite(pwm4, abs(p4));
//   if (p1>=0)
//     digitalWrite(dir1,LOW);
//   else
//     digitalWrite(dir1,HIGH);
//   if (p2>=0)
//     digitalWrite(dir2,LOW);
//   else
//     digitalWrite(dir2,HIGH);
//   if (p3>=0)
//     digitalWrite(dir3,LOW);
//   else
//     digitalWrite(dir3,HIGH);
//   if (p4>=0)
//     digitalWrite(dir4,LOW);
//   else
//     digitalWrite(dir4,HIGH);
// }
