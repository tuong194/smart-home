#include <Wire.h>
#include"DHT.h"
#include <LiquidCrystal_I2C.h>
#include<ArduinoJson.h>
//#include<SoftwareSerial.h>

DynamicJsonDocument JSdoc(1024);

LiquidCrystal_I2C lcd(0x27,16,2);

String inputString;

#define coi 3
#define gas 2
#define mua 4
//#define led 7
//#define chyendong 6
#define in1 6
#define in2 7

#define ctht1 11
#define ctht2 12


#define quat 8
#define suoi 9

#define btnMode 10
boolean state=HIGH;
int mode=0;

#define DHT11Pin 5
#define DHTType DHT11
DHT HT(DHT11Pin, DHTType);
float humi;  // do am
float tempC; // nhiet do C
float tempF; // do F

void setup() {
  Serial.begin(9600);
 // mySerial.begin(9600);
  pinMode(btnMode, INPUT_PULLUP);
  pinMode(coi,OUTPUT);
//  pinMode(led,OUTPUT);
//  pinMode(chuyendong, INPUT);
  pinMode(ctht1, INPUT);
  pinMode(ctht2, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  pinMode(quat,OUTPUT);
  pinMode(suoi,OUTPUT);
  pinMode(gas, INPUT);
  pinMode(mua, INPUT);
  
  HT.begin();
  lcd.init();
  lcd.backlight();
}

void loop() {

  gasf();
  rain();
  //temp();
 // chuyendong();

   if (digitalRead(btnMode) == LOW) { // nuts duoc nhan
    if (state == HIGH) {
      mode++;
      mode%=2;
   if(mode == 0){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("AUTO ON");
      JSdoc["auto1"]="ON";
      delay(3000);
     tempauto();
    
  }else if(mode == 1){
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("AUTO OFF");
     JSdoc["auto1"]="OFF";
     delay(3000);
     temp();
     readData();
     
  }
    }
    state = LOW;
  } else {
    state = HIGH;
  }

    sendData();
    
}


void gasf(){
  int giatri=digitalRead(gas);
 if(giatri == 0){
  tone(coi,1000); // tone(pin, tần số Hz)
  JSdoc["gas"]="Warning";
 }else{
  noTone(coi);
  JSdoc["gas"]="KO SAO";
 }
}
void rain(){
  int giatri2=digitalRead(mua);
  if(giatri2 == 0){ // mưa, kéo giàn phơi vào
    lui();
    if(digitalRead(ctht1) == 0 && digitalRead(ctht2) == 1){
      dung();
    }
    
 }else{ // tạnh thì kéo giàn phơi ra
  tien();
  if(digitalRead(ctht2) == 0 && digitalRead(ctht1) == 1){
    dung();
  }
 }
}

void tempauto(){
  humi = HT.readHumidity();
  tempC= HT.readTemperature();
  
  JSdoc["nhietdo"]=String(tempC);
  JSdoc["doam"]=String(humi);

  lcd.setCursor(0,0);
  lcd.print("NHIET DO ");
  lcd.setCursor(9,0);
  lcd.print(tempC);

  
  lcd.setCursor(0,1);
  lcd.print("DO AM ");
  lcd.setCursor(9,1);
  lcd.print(humi);

  if(tempC>30){
    digitalWrite(quat,HIGH);
    digitalWrite(suoi,LOW);
    JSdoc["quat"]="ON";
    JSdoc["suoi"]="OFF";
  }else if(tempC<25){
    digitalWrite(quat,LOW);
    digitalWrite(suoi,HIGH);
    JSdoc["quat"]="OFF";
    JSdoc["suoi"]="ON";
  }
  
}
void temp(){
  humi = HT.readHumidity();
  tempC= HT.readTemperature();
  tempF= HT.readTemperature(true);

  JSdoc["nhietdo"]=String(tempC);
  JSdoc["doam"]=String(humi);

  lcd.setCursor(0,0);
  lcd.print("NHIET DO ");
  lcd.setCursor(9,0);
  lcd.print(tempC);

  
  lcd.setCursor(0,1);
  lcd.print("DO AM ");
  lcd.setCursor(9,1);
  lcd.print(humi);

  if(digitalRead(quat)==HIGH){
    JSdoc["quat"]="ON";
  }else{
    JSdoc["quat"]="OFF";
  }
  if(digitalRead(suoi)==HIGH){
    JSdoc["suoi"]="ON";
  }else{
    JSdoc["suoi"]="OFF";
  }
}

void chuyendong(){ // cảm biến chuyển động
  int gt = digitalRead(chuyendong);
  if(gt==1){
    digitalWrite(led,HIGH);
  }else{
    digitalWrite(led,LOW);
  }
}
void tien(){
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
}
void lui(){
  digitalWrite(in2,HIGH);
  digitalWrite(in1,LOW);

}
void dung(){
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
}
void sendData(){
  //serializeJson(JSdoc,mySerial);
  serializeJson(JSdoc,Serial);

  delay(1000);
}
void readData(){
  while(Serial.available()){
    inputString = mySerial.readString();
  }
  if(inputString=="onQ"){
    digitalWrite(quat,HIGH);
  }else if(inputString=="offQ"){
    digitalWrite(quat,LOW);
  }else if(inputString=="onS"){
    digitalWrite(suoi,HIGH);
  }else if(inputString=="offS"){
    digitalWrite(suoi,LOW);
  }
}
