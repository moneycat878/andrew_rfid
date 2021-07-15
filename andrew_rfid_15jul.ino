#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN 9
#define SS_PIN 10

byte readCard[4];

// Define authorized cards
String MasterTag1 = "977AE2B5";
String MasterTag2 = "B760B5B4";
String MasterTag3 = "4719B1B5";
String accessCard1 = "D3AACB3E";
String accessCard2 = "03E3703F";
String accessCard3 = "E3B9FD3E";

// Define future string for user card
String inputCard = "";

bool adminMode = false; 

//Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Initiating
  Serial.begin(9600); //Initialize serial communications 
  SPI.begin(); 
  mfrc522.PCD_Init(); //Initialize MFRC522
  delay(4); //delay time after init for board to be ready
  
}

void adminModeTimer() {
  Serial.println("Scan desired card to add to system within the next 5 seconds...");
  //start 5 second timer to add a new card
  delay(5000);
  adminMode = false;
  Serial.println("Admin mode disabled. Scan a master tag again to add a new card");
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
  for ( uint8_t i = 0; i < 4; i++) { // The MIFARE PICCs that we use have 4 byte UID
  //readCard[i] = mfrc522.uid.uidByte[i];
  inputCard.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  
  inputCard.toUpperCase();
  Serial.println("User card ID presented: " + inputCard);
  mfrc522.PICC_HaltA(); // Stop reading
  return true;

}

void loop() {

  // Wait untill new tag is available
  if (getID()) {
     if (!adminMode){
            // user mode
            //check if input card matches any authorized cards. Only put user cards here.
            if (inputCard == accessCard1 || inputCard == accessCard2 || inputCard == accessCard3){
              Serial.println("Access granted");
              
              inputCard = ""; //reset the input card to blank
            }
            //check if input card matches any master tags. Only put master tags here.
            else if(inputCard == MasterTag1 || inputCard == MasterTag2 || inputCard == MasterTag3){
              //check if the card was a master tag
              bool adminMode = true;
              Serial.println("Access granted. Master tag used.");
              // start timer and enable admin mode to add new card
              adminModeTimer();
              
              inputCard = ""; //reset the input card to blank
            }
            else{
              Serial.println("Access denied");
              
              inputCard = ""; //reset the input card to blank
            }
     }
     else{
      // admin mode
      Serial.println("Master card used. Please scan new card to be authorized...");
      //accessCards.push(inputCard);
      Serial.println("New card addition functionally not implemented yet");
      adminMode = false;
     }
  }  
  else{
    Serial.println("Sensor standing by...");
  }
  
delay(2000);
// Loop begins again and waits for card to be scanned

}
      
