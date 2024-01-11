#include <Wire.h>
#include<EEPROM.h>
#include<LiquidCrystal_I2C.h>
#include<Keypad.h>
#include<Servo.h>
#include<SPI.h>
#include<MFRC522.h>

#define btn 6 // nut nhan chon mode
#define btnServo 4 // nut nha dong cua
#define servo 3

#define RST_PIN   5
#define SS_PIN    53

boolean stateServo = HIGH;
boolean state = HIGH;
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int mode = 0; // chon che do

/*-----------------------------------RFID----------------------------*/
int UID[4]; // UID cua the
int diachiMas = 1; // dia chi luu the master
int diachi = 5; // địa chỉ lưu các thẻ phụ
int a = 0; // số ô nhớ EEPROM da su dung
int m = 5, moi = 0; // de so sanh eeprom
int UIDcheck[4]; int gt_check = 0; int o = 0; // lưu tạm giá trị đọc từ EEPROM vào UIDcheck
boolean check = false; // co the
boolean checkNo = false; // ko co the


int n = 5; int n_moi = 0; // tim dia chi 0 dau tien
int sum_zero = 0;
int diachi_zero;


/*------------------------------Password---------------------------*/
char paw1[5]; // pass nhap tu ban phim
char pw[] = "1234"; // pass mo cua
int i = 0;
char oldPass[5]; int d1 = 0;int d2=0; // pass cu de doi mat khau

/*------------------------Keypad----------------------------------*/
const byte ROWS = 4;
const byte COLS = 4;
char MatrixKey[ROWS][COLS] =
{
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {13, 12, 11, 10}; // R1,R2,R3,R4
byte colPins[COLS] = {9, 8, 7}; // C1,C2,C3,C4

Keypad Mykeys = Keypad( makeKeymap(MatrixKey), rowPins, colPins, ROWS, COLS);




void nhap();  // nhap mat khau tu ban phim
void checkPass(); // kiem tra mat khau
void resetPass(); // doi mat khau

void checkMaster();
void CheckEEPROM();
void readRFID();
void zero();
void insert();
void clearAll();
void xoa1();

void setup() {
  Serial.begin(9600);
  pinMode(btn, INPUT_PULLUP);
  pinMode(btnServo, INPUT_PULLUP);
  SPI.begin();
  mfrc522.PCD_Init();

  myServo.attach(servo);
  myServo.write(0);

  lcd.init();
  lcd.backlight(); // bat den nen
  lcd.setCursor(0, 0);
  lcd.print(" HE THONG ");
  lcd.setCursor(0, 1);
  lcd.print("KHOA CUA TU DONG");
  delay(2000);
  lcd.clear();

  //EEPROM.write(0,0);
}

void loop() {

  pw[0] = EEPROM.read(1000);
  pw[1] = EEPROM.read(1001);
  pw[2] = EEPROM.read(1002);
  pw[3] = EEPROM.read(1003);

  if (digitalRead(btnServo) == LOW) { // nuts duoc nhan
    if (stateServo == HIGH) {
      if(myServo.read()==0){
        myServo.write(100);
      }else if(myServo.read()==100){
        myServo.write(0);
      }
    }
    stateServo = LOW;
  } else {
    stateServo = HIGH;
  }
  /*------------------Nhấn nút để chọn chế độ hoạt động*/
  if (digitalRead(btn) == LOW) { // nuts duoc nhan
    if (state == HIGH) {
      mode ++;
      mode %= 5;
      lcd.clear();
    }
    state = LOW;
  } else {
    state = HIGH;
  }
/*------------------Các chế độ như sau:----------------------------
 * mode = 0: Mở cửa bằng thẻ RFID
 * mode = 1: Mở cửa bằng mật khẩu 
 * mode = 2: Đổi mật khẩu
 * mode = 3: Thêm thẻ RFID
 * mode = 4: Xóa 1 thẻ bất kỳ
---------------------------------------------------------------------*/

  if(mode == 0){
    checkMaster();
  }else if(mode == 1){
    lcd.setCursor(0, 0);
    lcd.print(" NHAP MK        ");
    nhap();
    checkPass();
  }else if(mode == 2){
    resetPass();
  }else if(mode == 3){
    insert();
  }else if(mode == 4){
    xoa1();
  }
  

}

void nhap() {
  char EnterKey = Mykeys.getKey();
  if (EnterKey) {
    paw1[i] = EnterKey;
    // Nhập mật khẩu
    if (i == 0) {
      paw1[0] = EnterKey;
      lcd.setCursor(6, 1);
      lcd.print(paw1[0]);
      delay(500); // Ký tự hiển thị trên màn hình LCD trong 0.5s
      lcd.setCursor(6, 1);
      lcd.print("*"); // Ký tự được thay bởi dấu *

    }
    if (i == 1) {
      paw1[1] = EnterKey;
      lcd.setCursor(7, 1);
      lcd.print(paw1[1]);
      delay(500);
      lcd.setCursor(7, 1);
      lcd.print("*");

    }
    if (i == 2) {
      paw1[2] = EnterKey;
      lcd.setCursor(8, 1);
      lcd.print(paw1[2]);
      delay(500);
      lcd.setCursor(8, 1);
      lcd.print("*");

    }
    if (i == 3) {
      paw1[3] = EnterKey;
      lcd.setCursor(9, 1);
      lcd.print(paw1[3]);
      delay(500);
      lcd.setCursor(9, 1);
      lcd.print("*");

    }
    i++;
  }
}
void checkPass() {
  if (i == 4) {
    if (!strcmp(paw1, pw)) {
      lcd.clear();
      lcd.print("  Open  ");
      
      myServo.write(100);
      delay(3000);
//      for (int j = 5; j > 0; j--) {
//        lcd.setCursor(5, 1);
//        lcd.print(j);
//        delay(1000);
//      }
//      lcd.clear();
//      lcd.print("CLOSE");
//      myServo.write(0);
      
      i = 0;
    }
    if (strcmp(paw1, pw)) {
      lcd.clear();
      lcd.print("SAI PASSS");
      delay(3000);
      i = 0;
    }
  }
}

void resetPass() {
  lcd.setCursor(0, 0);
  lcd.print(F("DOI MAT KHAU    "));
  lcd.setCursor(0, 1);
  lcd.print("Pass cu ");
  ///////////////////////////////////////////////////////////////////////////////
  char EnterKey = Mykeys.getKey();
  if (EnterKey) {
    oldPass[d1] = EnterKey;
    // Nhập mật khẩu
    if (d1 == 0) {
      oldPass[0] = EnterKey;
      lcd.setCursor(9, 1);
      lcd.print(oldPass[0]);
    }
    if (d1 == 1) {
      oldPass[1] = EnterKey;
      lcd.setCursor(10, 1);
      lcd.print(oldPass[1]);
    }
    if (d1 == 2) {
      oldPass[2] = EnterKey;
      lcd.setCursor(11, 1);
      lcd.print(oldPass[2]);

    }
    if (d1 == 3) {
      oldPass[3] = EnterKey;
      lcd.setCursor(12, 1);
      lcd.print(oldPass[3]);
      delay(500);
    }
    d1++;
  }
  EnterKey = NO_KEY;
  ///////////////////////////////////////////////////////////////////////////////
  if (d1 == 4) {
    if (!strcmp(oldPass, pw)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DOI MAT KHAU    ");
      lcd.setCursor(0, 1);
      lcd.print("Pass moi ");

      /////////////////////////////////////
      while (d2 < 4) {
        EnterKey = Mykeys.getKey();
        if (EnterKey) {
          paw1[d2] = EnterKey;
          if (d2 == 0) {
            paw1[0] = EnterKey;
            lcd.setCursor(9, 1);
            lcd.print(paw1[0]);
                  EEPROM.write(1000,paw1[0]);
                  delay(50);
          }
          if (d2 == 1) {
            paw1[1] = EnterKey;
            lcd.setCursor(10, 1);
            lcd.print(paw1[1]);
                  EEPROM.write(1001,paw1[1]);
                  delay(50);
          }
          if (d2 == 2) {
            paw1[2] = EnterKey;
            lcd.setCursor(11, 1);
            lcd.print(paw1[2]);
                  EEPROM.write(1002,paw1[2]);
                  delay(50);
          }
          if (d2 == 3) {
            paw1[3] = EnterKey;
            lcd.setCursor(12, 1);
            lcd.print(paw1[3]);
                  EEPROM.write(1003,paw1[3]);
                  delay(50);
          }
          d2++;
        }
      }

      ////////////////////////////////////
      if (d2 == 4) {
        lcd.clear();
        lcd.print("DA XONG");
        delay(3000);
        d2 = 0;
        d1 = 0;
      }

    }
    else if (strcmp(oldPass, pw)) {
      lcd.clear();
      lcd.print("SAI PASS CU");
      delay(2500);
      d1 = 0;
    }
  }
}

// ktra xem co the mas chua
void checkMaster() {
  if (EEPROM.read(1) == 0) {
    lcd.clear();
    lcd.print("NHAP MASTER DI");
    if (mfrc522.PICC_IsNewCardPresent()) {
      if (mfrc522.PICC_ReadCardSerial()) {
        for (byte j = 0 ; j < mfrc522.uid.size; j++) {
          UID[j] = mfrc522.uid.uidByte[j];
          EEPROM.write(diachiMas, UID[j]);
          diachiMas++;
        }
        diachiMas = 1;
        
      }
    }
    mfrc522.PICC_HaltA();// Tạm ngừng đọc thẻ
    mfrc522.PCD_StopCrypto1(); // Ngừng việc sd RFRC522
    //delay(1500);
  } else {
    lcd.clear();
    lcd.print("HE THONG SS");
    readRFID();
  }
}

// doc gia tri tu eeprom va so sanh
void CheckEEPROM() {
  while (m < a) {
    moi = m + 4;
    for (m; m < moi; m++) {
      gt_check = EEPROM.read(m);
      if (o <= 3) {
        UIDcheck[o] = gt_check;
      }
      o++;
    }
    // Neu the co trong EEPROM
    if ((UID[0] == UIDcheck[0] && UID[1] == UIDcheck[1] && UID[2] == UIDcheck[2] && UID[3] == UIDcheck[3])){
      check = true; checkNo = false;
      break;
    } else if (UID[0] != UIDcheck[0] || UID[1] != UIDcheck[1] || UID[2] != UIDcheck[2] || UID[3] != UIDcheck[3]) {
      check = false; checkNo = true;
    }

    m = moi;
    o = 0;
  }
}

// doc va kiem tra the
void readRFID() {
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      for (byte j = 0; j < mfrc522.uid.size; j++) {
        //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        UID[j] = mfrc522.uid.uidByte[j];// đọc cái UID của thẻ
        //Serial.print(UID[i]);
      }
      //Serial.println("   ");
      a=EEPROM.read(0);
      CheckEEPROM();
      
      // the dung
      if (check == true || (UID[0] == EEPROM.read(1) && UID[1] == EEPROM.read(2) && UID[2] == EEPROM.read(3) && EEPROM.read(4))) {
        lcd.clear();
        lcd.print("MO CUA");
        myServo.write(100);
        delay(3000);
//        for (int j = 5; j > 0; j--) {
//          lcd.setCursor(5, 1);
//          lcd.print(j);
//          delay(1000);
//        }
//        myServo.write(0);
        
        m = 5; o = 0; moi = 0;
        check = false;
        
      } else if (check == false ) {
        lcd.clear();
        lcd.print("THE SAI");
        delay(3000);
        m = 5; o = 0; moi = 0;
        checkNo = false;
      }

    }
  }
  mfrc522.PICC_HaltA();// Tạm ngừng đọc thẻ
  mfrc522.PCD_StopCrypto1(); // Ngừng việc sd RFRC522
}

// tim dia chi co gia tri 0 dau tien
void zero() {
  //n=5; n_moi=0; sum_zero =0; diachi_zero=0;
  // a=EEPROM.read(0);
  while (n < a) {
    // kiểm tra 4 ô 1 lượt
    n_moi = n + 4;
    for (n; n < n_moi; n++) {
      sum_zero += EEPROM.read(n);
    }
    if (sum_zero == 0) {
      diachi_zero = n - 4;
    }
    n = n_moi;
    sum_zero = 0;
  }
  if (diachi_zero == 0) {
    diachi_zero = a;
    n = 5; n_moi = 0; sum_zero = 0;
  }
  //Serial.print("dia chi zero la: "); Serial.println(diachi_zero);

}

// them the moi
void insert() {
  a = EEPROM.read(0);
  diachi = 5;
  lcd.clear();
  lcd.print("THEM THE");
  if (a == 0) { // chua co the phu nao
    int diachi = 5; // bat dau luu vao o so 5 ,6 7,.....
    if (mfrc522.PICC_IsNewCardPresent()) {
      if (mfrc522.PICC_ReadCardSerial()) {
        for (byte j = 0; j < mfrc522.uid.size; j++) {
          
          UID[j] = mfrc522.uid.uidByte[j];
          
          UIDcheck[j] = UID[j];    
        }
        
        // neu la the Master thi ko luu
        if (UIDcheck[0] == EEPROM.read(1) && UIDcheck[1] == EEPROM.read(2) && UIDcheck[2] == EEPROM.read(3) && UIDcheck[3] == EEPROM.read(4)) {
         // Serial.println("a = 0 nhe, la the Mster");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("LA THE MASTER");
          lcd.setCursor(0, 1);
          lcd.print("   THU LAI");
          delay(2000);
        } else {
          for (byte j = 0; j < 4; j++) {
            EEPROM.write(diachi, UID[j]);
           
            a = diachi;
            diachi++;
          }
          EEPROM.write(0, a);
         
          lcd.clear();
          lcd.print("DANG LUU ...");
          delay(2000);
        }

      }
    }
    mfrc522.PICC_HaltA();// Tạm ngừng đọc thẻ
    mfrc522.PCD_StopCrypto1(); // Ngừng việc sd RFRC522

  } else if (a > 0) { // da co the phu
    if (mfrc522.PICC_IsNewCardPresent()) {
      if (mfrc522.PICC_ReadCardSerial()) {
        for (byte j = 0; j < mfrc522.uid.size; j++) {       
          UID[j] = mfrc522.uid.uidByte[j];     
          
          
        }
        CheckEEPROM(); // ktra xem the da co trong EEPROM chua
        // neu la the master
        if (UID[0] == EEPROM.read(1) && UID[1] == EEPROM.read(2) && UID[2] == EEPROM.read(3) && UID[3] == EEPROM.read(4)) {
          
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("LA THE MASTER");
          lcd.setCursor(0, 1);
          lcd.print("   THU LAI");
          delay(2000);
          m = 5; o = 0; moi = 0;
        }
        // the da co trong EEPROM
        else if (check == true && checkNo == false) {       
          lcd.clear();
          lcd.print("THE DA CO");
          delay(2000);
          m = 5; o = 0; moi = 0;
          check = false;
        }
        // neu chua co thi luu vao
        else if (check == false && checkNo == true) {
          zero(); // tim vi tri =0 dau tien de luu
          if (diachi_zero == a) {
            for (int j = 0; j < 4; j++) {
              diachi_zero++;
              EEPROM.write(diachi_zero, UID[j]);
              
            }
            a = diachi_zero;
            EEPROM.write(0, a);

          } else if (diachi_zero != a) {
            for (int j = 0; j < 4; j++) {
              EEPROM.write(diachi_zero, UID[j]);
              
              diachi_zero++;
            }
            diachi_zero = a;
          }

          lcd.clear();
          lcd.print("DANG LUU...");
          delay(2000);
          m = 5; o = 0; moi = 0; checkNo = false;
          n = 5; n_moi = 0; sum_zero = 0;
        }
      }
    }
    mfrc522.PICC_HaltA();// Tạm ngừng đọc thẻ
    mfrc522.PCD_StopCrypto1(); // Ngừng việc sd RFRC522
  }

}

//xoa 1 the bat ki
void xoa1() {
  lcd.clear();
  lcd.print("XOA 1 THE");
  a = EEPROM.read(0);
  if (a == 0) {
    lcd.clear();
    lcd.print("CHUA CO THE");
    delay(1500);
    mode = 0;
  }
  // co the phu roi
  else {
    if (mfrc522.PICC_IsNewCardPresent()) {
      if (mfrc522.PICC_ReadCardSerial()) {
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
          UID[i] = mfrc522.uid.uidByte[i];
          Serial.print(UID[i]);
        }
        Serial.println("");

        CheckEEPROM(); // ktra xem the da co trong EEPROM chua
        // neu la the master
        if (UID[0] == EEPROM.read(1) && UID[1] == EEPROM.read(2) && UID[2] == EEPROM.read(3) && UID[3] == EEPROM.read(4)) {
          Serial.println("la the MAster");
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("LA THE MASTER");
          lcd.setCursor(0, 1);
          lcd.print("KO THE XOA");
          delay(2000);
          m = 5; o = 0; moi = 0;
        }

        // the da co trong EEPROM
        else if (check == true && checkNo == false) {
          m = m - 4;
          for (int i = 0; i < 4; i++) {
            EEPROM.write(m, 0);
            m++;
          }
          lcd.clear();
          lcd.print("DA XOA THE!");
          delay(2000);
          m = 5; o = 0; moi = 0;
          check = false;
        }
        // neu chua co
        else if (check == false && checkNo == true) {
          lcd.clear();
          lcd.print("KO TIM THAY...");
          delay(2000);
          m = 5; o = 0; moi = 0; checkNo = false;
          n = 5; n_moi = 0; sum_zero = 0;
        }
      }
    }
    mfrc522.PICC_HaltA();// Tạm ngừng đọc thẻ
    mfrc522.PCD_StopCrypto1(); // Ngừng việc sd RFRC522
  }
}
