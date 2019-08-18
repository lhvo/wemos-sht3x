
#include <Wire.h>

#include "SHTSensor.h"

SHTSensor sht;


ADC_MODE(ADC_VCC);

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "ssid";
const char* password = "password";
const char* sensorName ="wemos";
const int duration = 300; /* duration in sec */
const int led = BUILTIN_LED;

float h=0;
float t =0;
float hic=0;
int acquired=0;
int count=0;
uint32_t delayMS;

void getData() {
  float lastH = h;
  float lastT = t;
  if (sht.readSample()) {
      h=sht.getHumidity();
      t=sht.getTemperature();
  } else {
      t=lastT;
      h=lastH;
  }
  digitalWrite(led, LOW);
}



void prepareSensor() {  
Wire.begin();

  if (sht.init()) {
      //Serial.print("init(): success\n");
  } else {
      //Serial.print("init(): failed\n");
  }
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
}

void setup(void){
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  prepareSensor();
  
  getData();

  WiFi.forceSleepWake(); delay(1);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  count=0;
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    count=count+1;
    Serial.print(".");
    if (count > 60)   
          {
           WiFi.mode(WIFI_OFF);delay(1);
           WiFi.forceSleepBegin();delay(1);
           ESP.deepSleep(duration* 1e6);  
          }
  }

  if (MDNS.begin(sensorName)) {
    Serial.println("MDNS responder started");
  }
  //Serial.println("Network ready");
  sendData();
  delay(10);
  WiFi.disconnect(); 
  WiFi.mode(WIFI_OFF);delay(1);
  WiFi.forceSleepBegin();delay(1);
  ESP.deepSleep(duration* 1e6);  
 
}

void sendData() {
       unsigned int vcc=ESP.getVcc();
       String message ="weather,sensor="""+String(sensorName)+""" humidity="+String(h)+",temperature="+String(t)+",heatIndex=0,vcc="+String(vcc);
       HTTPClient http;
       http.begin("http://pi1.local:8086/write?db=db");
       http.addHeader("Content-Type", "application/x-www-form-urlencoded");
       int httpCode=http.POST(message);
       Serial.println(message);
       http.end();
 
}
void loop(void){

}
