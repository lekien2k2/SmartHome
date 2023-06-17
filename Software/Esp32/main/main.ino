#include <Arduino.h>

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <SimpleDHT.h>
#include <HardwareSerial.h>

#define WIFI_SSID "HT"
#define WIFI_PASSWORD "Thiet123"

#define FIREBASE_HOST "https://doan1-efece-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "nDkFiYMAgCctztaQzRaNeJgxJAYubXe6BbicGCym"

//#define FIREBASE_HOST "https://damh1-c89a1-default-rtdb.firebaseio.com/"
//#define FIREBASE_AUTH "o1U6ZkdscursWsnGPIMrtMtjGJgcud4WBEXBXf5h"

//Use uart 2
HardwareSerial SerialPort(2);

FirebaseData fbdo;
int pinDHT11 = 32;
SimpleDHT11 dht11;

byte temperature = 0;
byte humidity = 0;
 
const int ledPin = 5; 
 
// Setting PWM led
const int freqLed = 5000; 
const int ledChannel = 0; 
const int resolution = 8; 

//curtain
int motor1Pin1 = 21; 
int motor1Pin2 = 18;
//int enable1Pin = 19;//14 19

// Setting PWM properties: curtain
//const int freqCurtain = 5000; //30000
//const int pwmChannel = 0;
////const int resolution = 8;
//int dutyCycleCurtain = 0;

//lock
//char number = ' ';
  
void setup(){
  // setup PWM channel: LED
  ledcSetup(ledChannel, freqLed, resolution);

  ledcAttachPin(ledPin, ledChannel);

  //curtain
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);

//  pinMode(enable1Pin, OUTPUT);

//  // setup PWM channel: CURTAIN
//  ledcSetup(pwmChannel, freqCurtain, resolution); 
//  ledcAttachPin(enable1Pin, pwmChannel);

  Serial.begin(9600);
  //Setting uart 2
  SerialPort.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");
}

int dutyCycleLight;
String saveCurtain = "OFF";
void loop(){
  //DHT
  readDHT11();
  Firebase.setDouble(fbdo, "/DOAN1/temperature",temperature);
  Firebase.setDouble(fbdo, "/DOAN1/humidity",humidity);

  //LED
  if(Firebase.getInt(fbdo, "/DOAN1/led"))
  {
    String temp = String(fbdo.stringData());
    Serial.println("Download success: " + temp);  
    dutyCycleLight = temp.toInt();
    
  }else {
    Serial.println("Download fail: " + String(fbdo.stringData())); 
  }
 
  ledcWrite(ledChannel, dutyCycleLight);   
    delay(15);
    
  //CURTAIN
  if(Firebase.getString(fbdo, "/DOAN1/curtain")) {
      if(String(fbdo.stringData()) != saveCurtain) {
        if (String(fbdo.stringData()) == "ON"){
          Serial.println("Open curtian");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, HIGH); 
          delay(550);
        }
        else if(String(fbdo.stringData()) == "OFF") {
          Serial.println("Close curtain");
          digitalWrite(motor1Pin1, HIGH);
          digitalWrite(motor1Pin2, LOW); 
          delay(550);
        }
        else {
          Serial.println("Stopped");
          digitalWrite(motor1Pin1, LOW);
          digitalWrite(motor1Pin2, LOW);
        }
        saveCurtain = String(fbdo.stringData());
      }
      else {
        Serial.println("Stopped");
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, LOW);
      }
  }

  //LOCK
  if (SerialPort.available())
  {
    char number = SerialPort.read();
    if (number == '0') {
      Serial.println("Lock closed");
      Firebase.setDouble(fbdo, "/DOAN1/countLocker", 0);
    }
    else if (number == '1') {
      Firebase.setDouble(fbdo, "/DOAN1/countLocker", 1);
      Serial.println("Lock opened");
    }
    else if (number == '2') {
      Serial.println("Wrong password");
      Firebase.setDouble(fbdo, "/DOAN1/countLocker", 2);
    }
  }
}

void readDHT11()
{
  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed.");
  }
  else
  {
    Serial.print((int)temperature); Serial.print(" *C, "); 
    Serial.print((int)humidity); Serial.println(" %");  
  }  
}
