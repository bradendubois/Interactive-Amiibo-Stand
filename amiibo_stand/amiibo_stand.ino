#include <WaveHC.h>
#include <WaveUtil.h>
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <stdio.h>

//#include <MemoryFree.h>

//Loading Strings into PROGMEM to save RAM
#include <avr/pgmspace.h>

#define IRQ 6 // this trace must be cut and rewired!
#define RESET 8

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

SdReader card; // This object holds the information for the card
FatVolume vol; // This holds the information for the partition on the card
FatReader root; // This holds the information for the volumes root directory
FatReader file; // This object represent the WAV file for a pi digit or period
WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time

//Setup()
uint32_t versiondata;

//My Added Variables
uint32_t lastcard = 0;
uint32_t currentcard = 1;
uint32_t CID = 0;
boolean introStarted = false;

//Loop()
uint32_t cardidentifier;
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
 
//////////////////////////////////// SETUP

void setup() {
  
  // set up Serial library at 115200 bps
  Serial.begin(115200);
    
  PgmPrintln("Amiibo Scanner");
  
  if (!card.init()) Serial.println("Card init. failed!");
  if (!vol.init(card)) Serial.println("No partition!");
  if (!root.openRoot(vol)) Serial.println("Couldn't open dir");
  
  PgmPrintln("Files found:");
  root.ls();
  
  // Find Adafruit RFID/NFC shield
  nfc.begin();

  versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print(F("Didn't find PN53x board"));
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print(F("Found chip PN5")); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print(F("Firmware ver. ")); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  //enable timeout waiting for cards
  nfc.setPassiveActivationRetries(50);

}

/////////////////////////////////// LOOP

void loop() {
  
  //Memory Checker
  
  //Serial.print(F("Memory Available = "));
  //Serial.println(freeMemory());
  
  // Wait for an ISO14443A type cards (Mifare, etc.). When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  cardidentifier = 0;

  // Read a card
  if (success) {
    
    // turn the four byte UID of a mifare classic into a single variable #
    cardidentifier = uid[3];
    cardidentifier <<= 8; cardidentifier |= uid[2];
    cardidentifier <<= 8; cardidentifier |= uid[1];
    cardidentifier <<= 8; cardidentifier |= uid[0];
    
    // Compare to the previous card
    lastcard = currentcard;
    currentcard = cardidentifier;
    
    /* Optional Debug Information
    if (lastcard != currentcard) {
      Serial.println("\n*****************\n");
      Serial.print(F("Last Amiibo:     #"));
      Serial.println(lastcard);
      Serial.print(F("Current Card:"));
      Serial.println(currentcard);
    }
    */
     
    // Try to read the Character info page (#21)
    uint8_t charID[32];
    success = nfc.mifareultralight_ReadPage (21, charID);

    // Successfully read the Character info page
    if (success) {
      
      // turn page 21 into a character ID
      CID = charID[6];
      CID <<= 8; CID |= charID[6];
      CID <<= 8; CID |= charID[5];
      CID <<= 8; CID |= charID[4];
      CID <<= 8; CID |= charID[3];
      CID <<= 8; CID |= charID[2];
      CID <<= 8; CID |= charID[1];
      CID <<= 8; CID |= charID[0];

      // Only need to print this out if it's different
      if (currentcard != lastcard) {
        Serial.print("*****************\nCharacter Number: ");
        Serial.println(CID);
      }

      // Haven't started the Intro *yet*; start it
      if (!introStarted) {

        char intro[30];  
        sprintf(intro, "%d", CID);
        strcat(intro, ".wav");
        Serial.println(intro);

        // Play the song corresponding to this CID, if it exists
        playfile(intro);

        // Mark that the first song for this CID is now playing
        if (wave.isplaying) {
           introStarted = true;
        }
      
      // Intro Complete; Start the main song
      } else if (!wave.isplaying) {

        char song[30];  
        sprintf(song, "%d", CID);
        strcat(song, "_song.wav");
        Serial.println(song);
        
        playfile(song);

      }
        
    } else {
      // Amiibo removed; Clear/reset all data
      wave.stop();
      lastcard = 999999999;
      currentcard = 999999998;
      introStarted = false;
    }
  } else {
    // No Amiibo; Clear/reset all data, wait for RFID card to show up!
    Serial.println(F("Waiting for an Amiibo ..."));wave.stop();
    lastcard = 999999999;
    currentcard = 999999998;
    introStarted = false;
  }
}

/////////////////////////////////// HELPERS

/*
* Open and start playing a WAV file
*/
void playfile(char *name) {
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  if (!file.open(root, name)) {
    PgmPrint("Couldn't open file ");
    Serial.print(name);
    PgmPrint("\n");
    return;
  }
  if (!wave.create(file)) {
    PgmPrintln("Not a valid WAV");
    return;
  }
  // ok time to play!
  wave.play();
}
