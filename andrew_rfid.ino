#include <deprecated.h>
#include <MFRC522.h>
#include <MFRC522Extended.h>
#include <require_cpp11.h>
#include <SPI.h>
#include <LiquidCrystal.h> //Liquid crystal used for optional LCD screen

#define RST_PIN 9
#define SS_PIN 10

// Define authorized cards
String MasterTag1 = "977AE2B5";
String OwnerCard1 = "";

// Define future string for user card
String inputCard = "";

//Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);



void setup() {
 boi 
  // Initiating
  SPI.begin(); //Initialize serial communications 
  mfrc522.PCD_Init(); //Initialize MFRC522

  Serial.println("Scan your card...");

}

void loop() {

  // Wait untill new tag is available
  while(getID()){
    
    if(inputCard == MasterTag1){
      Serial.println("Access granted");
    }
    else{
      Serial.println("Access denied");
    }

    delay(2000);
    // Loop begins again and waits for card to be scanned
    
    }   
}

// Read new tag if available. If this function is true, 
boolean getID(){
  
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
  return false;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
  return false;
  }
  inputCard = "";  // Reset the tag to blank
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  inputCard.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  inputCard.toUpperCase();
  Serial.println("User card ID presented:");
  Serial.print(inputCard);
  mfrc522.PICC_HaltA(); // Stop reading
  return true;

}
