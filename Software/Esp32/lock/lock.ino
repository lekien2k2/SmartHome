#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h>
#include <HardwareSerial.h>


#define LED_PIN 2    // LED chân 2
#define RELAY_PIN 4  // Relay chân 4

//Sử dụng uart 2
HardwareSerial SerialPort(2); 

const uint8_t RST_PIN = 15;     // Configurable, see typical pin layout above
const uint8_t SS_PIN = 5;     // Configurable, see typical pin layout above

const byte ROWS = 4;  // Số hàng của keypad
const byte COLS = 4;  // Số cột của keypad

// Định nghĩa các ký tự trên keypad
char keys[ROWS][COLS] = {
  {'1', '2', '3','A'},
  {'4', '5', '6','B'},
  {'7', '8', '9','C'},
  {'*', '0', '#','D'}
};

// Các chân của keypad
byte rowPins[ROWS] = {26, 25, 33, 32};  // hàng của keypad
byte colPins[COLS] = {13, 12, 14, 27};      // cột của keypad

Servo mySevo;

// Định nghĩa LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);  // Khởi tạo keypad
void setup() {
  Serial.begin(9600);
  //Cấu hình cho uart 2
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH);

  lcd.init();
  lcd.backlight();
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  mySevo.attach(2);
}

//Trạng thái: 
//0-> Đã đóng; 1->Đã mở
//2-> Đã mở sai
int countLocker = 0;

void loop() {
 
  // Yêu cầu nhập mã PIN
  lcd.setCursor(0, 0);
  lcd.print("Enter PIN: ");
  Serial.println("Đã khóa cửa");

  char pin[5];
  int index = 0;

  // Lặp cho đến khi nhập đủ 4 số
  while (index < 4) {
    // Kiểm tra xem có thẻ RFID nào được đặt lên đầu đọc không
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    checkid();
    index = 0;
  }
  
    char key = keypad.getKey();
    if (key != NO_KEY) {
      lcd.setCursor(11 + index, 0);
      lcd.print("*");
      pin[index] = key;
      index++;
    }
  }
  Serial.println(pin);
  // Kiểm tra mã PIN có đúng không
  if (strcmp(pin, "1234") == 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("The door open");
    countLocker = 1;
    //Gửi cho master
    SerialPort.print(countLocker);
    Serial.println("Đã mở cửa");
    
    mySevo.write(100);
    delay(593);
    mySevo.write(90);
    delay(7000); //3000

    mySevo.write(80);
    delay(460);
    mySevo.write(90);
    lcd.clear();
    lcd.setCursor(0, 0);
    countLocker = 0;
    //Gửi cho master
    SerialPort.print(countLocker);
    lcd.print("Enter PIN: ");
    Serial.println("Đã khóa cửa");
    
  } else {
    
    digitalWrite(RELAY_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong PIN!");
    countLocker = 2;
    //Gửi cho master
    SerialPort.print(countLocker);
    Serial.println("Nhập khóa sai");

    delay(7000);

    digitalWrite(RELAY_PIN, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    countLocker = 0;
    //Gửi cho master
    SerialPort.print(countLocker);
    lcd.print("Enter PIN: ");
    Serial.println("Đã khóa cửa");
  }
  delay(100);
}

void checkid(){
    // Đọc mã thẻ RFID
    String cardID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardID += String(mfrc522.uid.uidByte[i], HEX);
    }
    
    // Kiểm tra xem mã thẻ có đúng không
    if (cardID == "63b81f1") {
    lcd.clear();
    lcd.setCursor(0, 0);
    countLocker = 1;
    //Gửi cho master
    SerialPort.print(countLocker);
    lcd.print("The door open");
    Serial.println("Đã mở cửa");
    
    mySevo.write(100);
    
    delay(593);
    mySevo.write(90);
    delay(7000);

    mySevo.write(80);
    delay(460);
    mySevo.write(90);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN: ");
    countLocker = 0;
    //Gửi cho master
    SerialPort.print(countLocker);
    Serial.println("Đã khóa cửa");
    delay(500);
    } else {
      digitalWrite(RELAY_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong PIN!");

    countLocker = 0;
    //truyền cho Esp master
    SerialPort.print(countLocker);

    delay(7000);

    digitalWrite(RELAY_PIN, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN: ");
    countLocker = 0;
    //Gửi cho master
    SerialPort.print(countLocker);
    Serial.println("Đã khóa cửa");
    delay(500);
    }
    // In ra Serial Monitor mã thẻ đọc được
    Serial.println("Card ID: " + cardID);
      // Đặt thẻ hiện tại về trạng thái sẵn sàng cho lần đọc tiếp theo
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  }
