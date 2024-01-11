#include "mainweb.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include<ArduinoJson.h>
#include<SoftwareSerial.h>
const byte RX = D2;
const byte TX = D3;
SoftwareSerial mySerial = SoftwareSerial(RX, TX);



String inputString = "";

ESP8266WebServer server(80);
char* ssid = "XOM TRO";
char* pass = "deptraicogisai";

unsigned long t1 =0;


void setup() {
  Serial.begin(9600);
 mySerial.begin(9600);
  ConnectWifi();
  server.on("/",SendWebpage); 
  server.on("/getdata",ReadJSON);
  server.begin();  
}

void loop() {

  nhanJson();

  server.handleClient();

}
void ConnectWifi(){
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);// thiết lập chế độ Station
  WiFi.begin(ssid,pass);
  Serial.print("Connecting");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print("...");
  }
  Serial.println(WiFi.localIP());
}

void SendWebpage(){
  server.send(200,"text/html",MainPage);
}
void nhanJson(){
  if(mySerial.available()>0){
    inputString = mySerial.readString();
    Serial.println(inputString);
  }
}
// nhận data từ UNO
void ReadJSON(){
  if(mySerial.available()>0){
    inputString = mySerial.readString();
  }
  String s = inputString;
  server.send(200,"application/json",s);
}
