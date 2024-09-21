/*
___RFID Sensor PIN setup:
3.3V  3.3V POWER
RST   DIGITAL PIN 9
GND   GND POWER
RQ    UNCONNECTED 
MISOI DIGITAL PIN 12
MOSI  DIGITAL PIN 11
SCK   DIGITAL PIN 13
SDA   DIGITAL PIN 10


___BiColor LED PIN setup:
Connect to GND and to the digital 5 and 4 pins.
Connect left pin to GND, middle to digital pin 5, and right to digital pin 4.
Make sure the led either has a built in resistor or if you need to attach one.
Colors: 0=off 1=green 2=red

___Unlock pushbutton PIN setup:
Connect one button side to a 220 ohm resistor, then to GND.
Connect the other side of the button to 3.3v power.
Connect a signal wire to read in between the button and resistor side to digital pin 2.

___Relay setup:
DC+ to Vin 
DC- to GND
IN to DIGITAL PIN 2 (or any other open digital pin)

*/

#include <MFRC522.h>
#include <SPI.h>
#include <BiColorLED.h>

#define RST_PIN 9
#define SS_PIN 10

int buttonPIN = 2;
int buzzer = 6;
int buttonState = 0;
int relayPIN = 3; //define pin to relay

//Relay setup:


BiColorLED biLED=BiColorLED(4, 5);

byte readCard[4];

// Define authorized cards
String MasterTag1 = "977AE2B5";
String MasterTag2 = "B760B5B4";
String MasterTag3 = "4719B1B5";
String accessCard1 = "D3AACB3E";
String accessCard2 = "03E3703F";
String accessCard3 = "E3B9FD3E";

size_t authorizedCardsSize = 1;
size_t masterCardsSize = 3;

// Array holding the authorized chards
unsigned long *authorizedCards;
unsigned long *masterCards;

// Define future string for user card
String inputCard = "";

bool adminMode = false; 

//Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  // Initiating
  Serial.begin(9600); //Initialize serial communications 
  pinMode(buzzer, OUTPUT);

  // Dynamically allocate memory for authorized and master card arrays
  authorizedCards = (unsigned long *)malloc(authorizedCardsSize * sizeof(unsigned long));
  masterCards = (unsigned long *)malloc(masterCardsSize * sizeof(unsigned long));

  // Added the default cards to the list
  authorizedCards[0] = hexStringToNumber(accessCard1);
  masterCards[0] = hexStringToNumber(MasterTag1);
  masterCards[1] = hexStringToNumber(MasterTag2);
  masterCards[2] = hexStringToNumber(MasterTag3);
  
  SPI.begin(); 
  mfrc522.PCD_Init(); //Initialize MFRC522
  delay(4); //delay time after init for board to be ready
  pinMode(buttonPIN, INPUT);
  lockDoor(); 
  
}

void unlockDoor(){
  biLED.setColor(1); 
  Serial.println("-=- Door unlocked");
  //send signal to relay and unlock door for 5 seconds
  digitalWrite(3, HIGH);
  delay(7000);
  //loop function will re-lock door
}

void lockDoor(){
  biLED.setColor(2); 
  Serial.println("=-= Door locked");
  //turn off signal to relay 
  digitalWrite(3, LOW);
}

void adminModeTimer() {
  Serial.println("Scan desired card to add to system within the next 5 seconds...");

  //start 5 second timer to add a new card
  for (int i = 0; i < 10; i++) {
    if (getID()) {
        Serial.println("Master card used. Card ID to be added to the authorized List: " + inputCard);
        playListModifiedBuzzer();
        addNewCard(inputCard);
        break;
    }

    delay(500);
  }

  Serial.println("Admin mode disabled");
  inputCard = ""; // reset the inputCard to blank
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

void playGrantedBuzzer() {
    tone(buzzer, 2000);
    delay(500);
    noTone(buzzer);
}

void playMasterBuzzer() {
    tone(buzzer, 2000);
    delay(500);
    noTone(buzzer);
    delay(500);

    tone(buzzer, 2000);
    delay(100);
    noTone(buzzer);
    delay(100);
}

void playListModifiedBuzzer() {
    tone(buzzer, 2000);
    delay(100);
    noTone(buzzer);
    delay(100);
}

void playDeniedBuzzer() {
    tone(buzzer, 1000);
    delay(100);
    noTone(buzzer);
    delay(100);
    
    tone(buzzer, 1000);
    delay(100);
    noTone(buzzer);
}

// Function to add a new card to authorized list
void addNewCard(String newCard) {
  // Check if the new card already exists in authorizedCards
  for (int i = 0; i < authorizedCardsSize; i++) {
    if (hexStringToNumber(newCard) == authorizedCards[i]) {
      Serial.println("Card already exists in authorized list.");
      return;
    }
  }

  // + authorizedCards array to accommodate the new card
  authorizedCardsSize++;
  authorizedCards = (unsigned long *)realloc(authorizedCards, authorizedCardsSize * sizeof(unsigned long));

  // Add the new card to authorizedCards
  authorizedCards[authorizedCardsSize - 1] = hexStringToNumber(newCard);

  Serial.println("New card added to authorized list.");
}

void loop() {
  
  //check if indoor unlock button is pressed
  buttonState = digitalRead(buttonPIN);
  if(buttonState == HIGH){
    Serial.println("Access granted");
    unlockDoor();
  }

  // Wait untill new tag is available
  if (getID()) {
            //check if input card matches any authorized cards. Only put user cards here.
            for (int i = 0; i < authorizedCardsSize; i++) {
              Serial.print("Checking against card number: ");
              Serial.println(authorizedCards[i], HEX);
              if (hexStringToNumber(inputCard) == authorizedCards[i]){
                Serial.println("Access granted");
                 // activate the buzzer
                playGrantedBuzzer();
                
                unlockDoor();
                
                inputCard = ""; //reset the input card to blank
                return;
              }
            }
            
            //check if input card matches any master tags. Only put master tags here.
            for (int i = 0; i < masterCardsSize; i++) {
              if (hexStringToNumber(inputCard) == masterCards[i]) {
                bool adminMode = true;
                Serial.println("Access granted. Master tag used.");
                inputCard = ""; //reset the input card to blank
                // activate the buzzer
                playMasterBuzzer();
                
                unlockDoor(); 
                
                // start timer and enable admin mode to add new card
                adminModeTimer();
                return;
              }
            }

            Serial.println("Access denied");

            // activate the buzzer
            playDeniedBuzzer();
            inputCard = "";
            
  }  
  else{
    Serial.println("Sensor standing by...");
  }
  
  delay(2000);
  lockDoor();
  // Loop begins again and waits for card to be scanned

}
      
