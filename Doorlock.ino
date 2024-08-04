#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>


#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte masterCard[4] = {0xC0, 0xF3, 0xD1, 0x1D};
byte masterCard1[4] = {0xFA, 0x92, 0x09, 0x6F};



int lockOutput = 7;
int greenLED = 5;
int redLED = 4;
int buttonPin = 6;


void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  finger.begin(57600);

  lcd.begin(16, 2);
  lcd.backlight();
  printWelcomeMessage();

  pinMode(lockOutput, OUTPUT);
  digitalWrite(lockOutput, HIGH);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buttonPin, INPUT);
  
}

void loop() {
      int buttonState = digitalRead(buttonPin);{
  
  if (buttonState == HIGH) {
    digitalWrite(lockOutput, LOW);
    delay(5000);
    
    digitalWrite(lockOutput, HIGH);
    resetArduino();
  } 
      }


  if (scanFingerprintAndCard()) {
    
    openDoor();
    delay(5000);
    closeDoor();
    resetArduino();

  }
}

bool scanFingerprintAndCard() {
  while (!finger.getImage()) {
  }

  uint8_t p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return false;

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK ) {
    displayMessage("Place your RFID card...");
    
    while (!rfid.PICC_IsNewCardPresent()) {
      delay(100);
      
    }
    printWelcomeMessage();
    if (rfid.PICC_ReadCardSerial()) {
      for (byte i = 0; i < 4; i++) {
        if (rfid.uid.uidByte[i] != masterCard[i] && rfid.uid.uidByte[i] != masterCard1[i]) {

          
          digitalWrite(redLED, HIGH);
          displayMessage(" Access Denied!");
          delay(2000);
          digitalWrite(redLED, LOW);
          printWelcomeMessage();
          return false;
          
        }
          
      }
      return true;
      
    }
  }

  return false;
}

void openDoor() {
  digitalWrite(lockOutput, LOW);
  digitalWrite(greenLED, HIGH);
  displayMessage(" Access Granted!");
  delay(3000);
  printWelcomeMessage();

}

void closeDoor() {
  digitalWrite(lockOutput, HIGH);
  digitalWrite(greenLED, LOW);

}

void displayMessage(const char* message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void printWelcomeMessage() {
  displayMessage("<Access Control>");
  lcd.setCursor(0, 1);
  lcd.print("Scan Your Fingerprint!");
}

void resetArduino() {
  asm volatile ("  jmp 0");
}

