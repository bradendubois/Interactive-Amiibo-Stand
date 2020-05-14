#include <WaveHC.h>
#include <WaveUtil.h>
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <stdio.h>

//#include <MemoryFree.h>

//Loading Strings into PROGMEM to save RAM
#include <avr/pgmspace.h>

char buffer[30];

#define IRQ 6 // this trace must be cut and rewired!
#define RESET 8

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

SdReader card; // This object holds the information for the card
FatVolume vol; // This holds the information for the partition on the card
FatReader root; // This holds the information for the volumes root directory
FatReader file; // This object represent the WAV file for a pi digit or period
WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time
/*
* Define macro to put error messages in flash memory
*/
#define error(msg) error_P(PSTR(msg))

//Setup()
uint32_t versiondata;

//My Added Variables
uint32_t lastcard = 0;
uint32_t currentcard = 1;
uint32_t CID = 0;
boolean  introPlaying = false;

//Loop()
uint32_t cardidentifier = 0;
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
 
//////////////////////////////////// SETUP

void setup() {
  // set up Serial library at 115200 bps
  Serial.begin(115200);
    
  PgmPrintln("Amiibo Scanner");
  
  if (!card.init()) {
    error("Card init. failed!");
  }
  if (!vol.init(card)) {
    error("No partition!");
  }
  if (!root.openRoot(vol)) {
    error("Couldn't open dir");
  }
  
  PgmPrintln("Files found:");
  root.ls();
  
  // find Adafruit RFID/NFC shield
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

unsigned digit = 0;

void loop() {
  
  //Memory Checker
  
  //Serial.print(F("Memory Available = "));
  //Serial.println(freeMemory());
   
  // wait for RFID card to show up!
  Serial.println(F("Waiting for an Amiibo ..."));
  
  // Wait for an ISO14443A type cards (Mifare, etc.). When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  cardidentifier = 0;
  CID = 999;  // This probably doesn't need to be here?
  if (success) 
  {
    // Found a card!

    Serial.print(F("Amiibo detected #"));
    // turn the four byte UID of a mifare classic into a single variable #
    cardidentifier = uid[3];
    cardidentifier <<= 8; cardidentifier |= uid[2];
    cardidentifier <<= 8; cardidentifier |= uid[1];
    cardidentifier <<= 8; cardidentifier |= uid[0];
    Serial.println(cardidentifier);
    
    //Check the previous card
    
    (lastcard = currentcard);
    (currentcard = cardidentifier);
    
    Serial.print(F("Last Amiibo:    #"));
    Serial.println(lastcard);
    Serial.print(F("Current Card:"));
    Serial.println(currentcard);
    
    // Check Character ID
    
      // Try to read the Character info page (#21)
      uint8_t charID[32];
      success = nfc.mifareultralight_ReadPage (21, charID);
      
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
      Serial.println("Character Number: ");
      Serial.println(CID);
      
      // Amiibo hasn't been removed
      if (currentcard == lastcard) {


        // Haven't started the into *yet*; start it
        if (!introPlaying) {

          // Convert the CID to the intro filename  
          char* introFileName = intro_ify(CID);

          // Play the song corresponding to this CID, if it exists
          playfile(introFileName);

          // Mark that the first song for this CID is now playing
          if (wave.isplaying) {
             introPlaying = true;
          }
            
          if(!wave.isplaying) 
          {
            introPlaying = false;
          }

          //Captain Falcon Song
          if (CID == 6)    
          
          //Samus Song
          if (CID == 49157)      
          {strcpy_P(buffer, (char*)pgm_read_word(&(string_table[31])));
            playfile(buffer);
            if (wave.isplaying) {
               introPlaying = true;
            }
          }
          
          
          //Little Mac = 49158)      
            
          //Fox == 32773)      
            
         //Wii Fit == 7)      
            
          //Villager == 32769)      
            
          //Marth == 33)      

          //Yoshi == 768)      

          //Pit == 16391)      

          //Kirby == 31)      

          //Pikachu == 6425)      

          //Link == 1)      

          //Luigi == 256)      

          //Diddy == 2304)      

          //DK == 2048)      

          //Peach == 512)      

          //Zelda == 257)      

          //Mario == 0)      

          //Toonlink == 65537)      

          //Dedede == 543)      

          //Lucario == 49178)      

          //Bowser == 1280)      

          //Mega Man == 32820)      

          //Sonic == 50)      

          //Rosalina == 66560)      

          //MetaKnight == 287)      

          //Shulk == 16418)      

          //Sheik == 65793)      

          //Ike == 289)      

          //Robin == 801)      

          //Lucina == 545)      

          //Charizard == 1561)      

          //Wario == 1792)      

          //Pac-Man == 16435)      

          //Ness == 32802)      

          //Gannondorf == 66049)      

          //Greninja == 37403)      

          //Jigglypuff == 10009)      

          //Dark Pit == 16647)      

          //Palutena == 16903)      

          //Zero Suit == 114693)      

          //Dr Mario == 65536)      
          
          //Bowser Jr == 1536)      

          //Olimar Song == 81926)      

          //Mr Game & Watch == 32775)      

          //Rob == 33031)      

          //Duck Hunt == 33287)      

          //Mii Brawler == 49159)      

          //Mii Swordfighter == 114695)      

          //Mii Gunner == 180231)      

          //Mewtwo == 38425)      

          //Falco == 33029)      

          //Lucas == 33058)      

          //Shovel Knight == 49205)      

          //Roy == 1234)      

          //Ryu == 1234)      

          //Cloud == 1234)      

          //Corrin == 1234)      

          //Bayonetta == 1234)      

          //Inkling Girl == 65544)      

          //Inkling Boy == 131080)      

          //Squid == 196616)      

          //Chibi Robo == 49186)      

          //Yarn Yoshi == 33620736)      

          //Isabelle == 98561)      

          //Tom Nook == 33537)      

          //KK Song == 33281)      

          //Mabel == 34817)      

          //Digby == 35841)      

          //Lottie == 49409)      

          //Cyrus Song == 35585)      

          //Reese == 35329)      

          //Blathers == 37377)      

          //Celeste == 37633)      

          //Resetti == 36353)      

          //Kicks == 37889)      

        
          
      }
      /*
        There is no song playing, play the intro sequence!
      */
      else 
      {
        //Captain Falcon
        if (CID == 6) 
        {
          strcpy_P(buffer, (char*)pgm_read_word(&(string_table[0])));
          playcomplete(buffer);
        }
        //Samus
        if (CID == 49157) 
        {strcpy_P(buffer, (char*)pgm_read_word(&(string_table[1])));
          playcomplete(buffer);
        }
        
      }
    }
    else 
    {

      wave.stop();
      lastcard = 999999999;
      currentcard = 999999998;
       introPlaying = false;
    }
  } 

  //If the Amiibo is gone, stop the song.
  else 
    {
      //if (currentcard == lastcard) 
      //{
        //Serial.println("UID else");
        wave.stop();
      //}  
      lastcard = 999999999;
      currentcard = 999999998;
       introPlaying = false;
    }  
}

/////////////////////////////////// HELPERS

/*
* print error message and halt
*/
void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}

/*
* print error message and halt if SD I/O error
*/
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}

/*
* Play a file and wait for it to complete
*/
void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying);
  
  // see if an error occurred while playing
  sdErrorCheck();
}
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

/*
 * Turn the ID into the appropriate filename that should be loaded
 *  for an Intro.
 */
char* intro_ify(int id) {
  char suffix[] = ".wav";
  return id_append(id, suffix);
}

/*
 * Turn the ID into the appropriate filename that should be loaded
 *  for a continuing song.
 */
char* song_ify(int id) {
  char suffix[] = "_song.wav";
  return id_append(id, suffix);
}

/*
 * Append an int and some arbitrary suffix into a char*
 */
char* id_append(int id, char* suffix) {
  char* intro[30];
  sprintf(intro, "%d", id);
  strcat(intro, suffix);
  return intro;
}
