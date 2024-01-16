#include"main.h"
#include"DHT.h"
#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal_I2C.h>
#include<ArduinoJson.h>


DynamicJsonDocument JSdoc(1024);
LiquidCrystal_I2C lcd(0x27, 16, 2);


WebServer server(80);

//STA
//char* ssid = "XOM TRO";
//char* pass = "deptraicogisai";

char* ssid = "iPhone"; //IP 172.20.10.10
char* pass = "hoilamgi";

unsigned long t1 = 0;

#define coi 0
#define gas 2

#define mua 15
#define ctht1 4
#define ctht2 16

#define in1 17 //L298N
#define in2 5


#define quat 18 // chan in3 L298N
#define suoi 19

#define btnMode 23 // chọn chế độ auto
boolean state = HIGH;
int mode = 0;

#define DHT11Pin 13
#define DHTType DHT11
DHT HT(DHT11Pin, DHTType);
float humi;  // do am
float tempC; // nhiet do C
float tempF; // do F


void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();

  pinMode(btnMode, INPUT_PULLUP);
  pinMode(coi, OUTPUT);

  pinMode(ctht1, INPUT);
  pinMode(ctht2, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(quat, OUTPUT);
  pinMode(suoi, OUTPUT);
  pinMode(gas, INPUT);
  pinMode(mua, INPUT);

  digitalWrite(quat, LOW);
  digitalWrite(suoi, HIGH);
  HT.begin();

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
  gasf();
  rain();
  temp();

  if (digitalRead(btnMode) == LOW) { // nuts duoc nhan
    if (state == HIGH) {
      mode++;
      mode %= 2;
    }
    state = LOW;
  } else {
    state = HIGH;
  }
  if (mode == 0) {
    JSdoc["auto1"] = "ON";

    if (tempC >= 25) {
      digitalWrite(quat, HIGH);
      digitalWrite(suoi, LOW);
      JSdoc["quat"] = "ON";
      JSdoc["suoi"] = "OFF";
    } else if (tempC < 24) {
      digitalWrite(quat, LOW);
      digitalWrite(suoi, HIGH);
      JSdoc["quat"] = "OFF";
      JSdoc["suoi"] = "ON";
    }

  } else if (mode == 1) {
    JSdoc["auto1"] = "OFF";
    //readData();
  }
  quatsuoi();
  
  server.handleClient();
}
void ConnectWifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);// thiết lập chế độ Station
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print(WiFi.localIP());
}

void gasf() {
  int giatri = digitalRead(gas);
  if (giatri == 0) {
    digitalWrite(coi,LOW); // tone(pin, tần số Hz)
    JSdoc["gas"] = "Warning";
  } else {
    digitalWrite(coi,HIGH);
    JSdoc["gas"] = "KO SAO";
  }
}
void rain() {
  int giatri2 = digitalRead(mua);
  if (giatri2 == 0) { // mưa, kéo giàn phơi vào
    lui();
    if (digitalRead(ctht1) == 0 && digitalRead(ctht2) == 1) {
      dung();
    }

  } else { // tạnh thì kéo giàn phơi ra
    tien();
    if (digitalRead(ctht2) == 0 && digitalRead(ctht1) == 1) {
      dung();
    }
  }
 
}

void temp() {
  humi = HT.readHumidity();
  tempC = HT.readTemperature();

  JSdoc["nhietdo"] = String(tempC);
  JSdoc["doam"] = String(humi);


  lcd.setCursor(0, 0);
  lcd.print("NHIET DO ");
  lcd.setCursor(9, 0);
  lcd.print(tempC);


  lcd.setCursor(0, 1);
  lcd.print("DO AM ");
  lcd.setCursor(9, 1);
  lcd.print(humi);


}
void quatsuoi() {
  if (digitalRead(quat) == HIGH) {
    JSdoc["quat"] = "ON";
  } else {
    JSdoc["quat"] = "OFF";
  }
  if (digitalRead(suoi) == HIGH) {
    JSdoc["suoi"] = "ON";
  } else {
    JSdoc["suoi"] = "OFF";
  }
}

void tien() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}
void lui() {
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);

}
void dung() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}



void onQ() {
  digitalWrite(quat,HIGH);
  server.send(200, "text/html", MainPage);
  Serial.println("onQ");
}
void offQ() {
  digitalWrite(quat,LOW);
  server.send(200, "text/html", MainPage);
  Serial.println("offQ");
}
void onS() {
  digitalWrite(suoi,HIGH);
  server.send(200, "text/html", MainPage);
  Serial.println("onS");
}
void offS() {
  digitalWrite(suoi,LOW);
  server.send(200, "text/html", MainPage);
  Serial.println("offS");
}

void SendWebpage() {
  server.send(200, "text/html", MainPage);
}

void ReadJSON() {
  String s;
  serializeJson(JSdoc, s);
  Serial.println(s);
  delay(1000);
   server.send(200,"application/json",s);
   s="";
}
