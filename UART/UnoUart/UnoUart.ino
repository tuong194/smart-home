#include <Wire.h>
#include"DHT.h"
#include <LiquidCrystal_I2C.h>
#include<ArduinoJson.h>

unsigned long t1 = 0;

DynamicJsonDocument JSdoc(1024);

LiquidCrystal_I2C lcd(0x27, 16, 2);

String inputString;

#define coi 3
#define gas 2

#define mua 4
#define ctht1 11
#define ctht2 12

#define in1 6 //L298N
#define in2 7


#define quat 8 // chan in3 L298N
#define suoi 9

#define btnMode 10 // chọn chế độ auto
boolean state = HIGH;
int mode = 0;

#define DHT11Pin 5
#define DHTType DHT11
DHT HT(DHT11Pin, DHTType);
float humi;  // do am
float tempC; // nhiet do C
float tempF; // do F

void setup() {
  Serial.begin(9600);

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
  lcd.init();
  lcd.backlight();
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
    readData();
  }
  quatsuoi();
  sendData();

}


void gasf() {
  int giatri = digitalRead(gas);
  if (giatri == 0) {
    tone(coi, 500); // tone(pin, tần số Hz)
    JSdoc["gas"] = "Warning";
  } else {
    noTone(coi);
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
void sendData() {
  serializeJson(JSdoc, Serial);
  delay(1000);
  //    if (millis() - t1 > 1000) {
  //
  //       t1 = millis();
  //    }
}

void readData() {
  while (Serial.available()) {
    inputString = Serial.readString();
  }
  if (inputString == "onQ") {
    digitalWrite(quat, HIGH);
  } else if (inputString == "offQ") {
    digitalWrite(quat, LOW);
  } else if (inputString == "onS") {
    digitalWrite(suoi, HIGH);
  } else if (inputString == "offS") {
    digitalWrite(suoi, LOW);
  }
}
