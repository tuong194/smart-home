#include "mainweb.h"
#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>

#include<ArduinoJson.h>


DynamicJsonDocument doc(1024);
LiquidCrystal_I2C lcd1(0x27, 16, 2);
String inputString = "";

WebServer server(80);
//STA
char* ssid = "XOM TRO";
char* pass = "deptraicogisai";

//access point
//const char* ssid = "ESP32Wifi";
//const char* pass = "12345";
//IPAddress local_ip(192,168,4,1);
//IPAddress gateway(192,168,4,1);
//IPAddress subnet(255,255,255,0);

unsigned long t1 = 0;
  String stemp="";
  String shumi="";

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  
  lcd1.init();
  lcd1.backlight();
  
  ConnectWifi();
  server.on("/", SendWebpage);
  server.on("/onQuat", onQ);
  server.on("/offQuat", offQ);
  server.on("/onSuoi", onS);
  server.on("/offSuoi", offS);
  server.on("/getdataJS", ReadJSON);


  
  server.begin();
}

void loop() {
  lcd1.setCursor(0, 0);
  lcd1.print("NHIET DO ");
  lcd1.setCursor(9, 0);
  lcd1.print(stemp);


  lcd1.setCursor(0, 1);
  lcd1.print("DO AM ");
  lcd1.setCursor(9, 1);
  lcd1.print(shumi);

  server.handleClient();

}
//void ConnectWifi(){
//  WiFi.softAP(ssid,pass);
//  WiFi.softAPConfig(local_ip,gateway,subnet);
//  delay(100);
//  Serial.println("thanh cong");
//}

void ConnectWifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);// thiết lập chế độ Station
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print(WiFi.localIP());
}

void onQ() {
  Serial2.print("onQ");
  server.send(200, "text/html", MainPage);
  Serial.print("onQ");
}
void offQ() {
  Serial2.print("offQ");
  server.send(200, "text/html", MainPage);
  Serial.print("offQ");
}
void onS() {
  Serial2.print("onS");
  server.send(200, "text/html", MainPage);
  Serial.print("onS");
}
void offS() {
  Serial2.print("offS");
  server.send(200, "text/html", MainPage);
  Serial.print("offS");
}

void SendWebpage() {
  server.send(200, "text/html", MainPage);
}


void ReadJSON() {
  if (millis() - t1 > 0) {
    while (Serial2.available() > 0) {
      inputString = Serial2.readString();
    }
    t1 = millis();
  }
   DeserializationError error = deserializeJson(doc,inputString); // decode chuoi json
    if(!error){
       stemp = doc["nhietdo"].as<String>();
       shumi = doc["doam"].as<String>();
    }
  String s = inputString;  
  Serial.println(s);
//String s2="{\"gas\":\"WARNING\",\"nhietdo\":\"25.50\",\"doam\":\"84.00\",\"auto1\":\"ON\",\"quat\":\"ON\",\"suoi\":\"OFF\"}";
//Serial.println(s2);
//delay(2000);

  server.send(200, "application/json", s);
  s="";
  
}

void nhanJson(){

  if(Serial2.available()>0){
    inputString = Serial2.readString();

    DeserializationError error = deserializeJson(doc,inputString); // decode chuoi json
    if(!error){
       stemp = doc["nhietdo"].as<String>();
       shumi = doc["doam"].as<String>();
    }
  }
    lcd1.setCursor(0, 0);
  lcd1.print("NHIET DO ");
  lcd1.setCursor(9, 0);
  lcd1.print(stemp);


  lcd1.setCursor(0, 1);
  lcd1.print("DO AM ");
  lcd1.setCursor(9, 1);
  lcd1.print(shumi);
  inputString="";
}
