/*

  ESP32 Dev Modeule version.

  Use the RDA5907FP device to deal with RDS and interrupt control.

  This sketch uses an ESP32 with LCD16X02 DISPLAY
  It is also a FM receiver capable to tune your local FM stations.
  This sketch saves the latest status of the receiver into the Atmega328 eeprom.

  TO RESET the EEPROM: Turn your receiver on with the encoder push button pressed.

  Read more on https://pu2clr.github.io/RDA5807/

  Wire up ESP32 Dev Module, RDA5807 and LCD16x02 or LCD16x04

  | Device name               | Device Pin / Description  |  Arduino Pin  |
  | --------------------------| --------------------      | ------------  |
  |    LCD 16x2 or 20x4       |                           |               |
  |                           | D4                        |  GPIO18       |
  |                           | D5                        |  GPIO17       |
  |                           | D6                        |  GPIO16       |
  |                           | D7                        |  GPIO15       |
  |                           | RS                        |  GPIO19       |
  |                           | E/ENA                     |  GPIO23       |
  |                           | RW & VSS & K (16)         |  GND          |
  |                           | A (15) & VDD              |  +Vcc         |
  | --------------------------| ------------------------- | --------------|
  | RDA5807                   |                           |               | 
  |                           | VCC                       |  3.3V         |
  |                           | SDIO / SDA (pin 8)        |  GPIO21       |
  |                           | SCLK (pin 7)              |  GPIO22       |
  |                           | GPIO2 (Pin 16)            |  GPIO34       |
  | Buttons                   |                           |               |
  |                           | Volume Up                 |  GPIO32       |
  |                           | Volume Down               |  GPIO33       |
  |                           | Stereo/Mono               |  GPIO25       |
  |                           | RDS ON/off                |  GPIO26       |
  | --------------------------| --------------------------| --------------|
  | Encoder                   |                           |               |
  |                           | A                         |  GPIO13       |
  |                           | B                         |  GPIO14       |
  |                           | PUSH BUTTON (encoder)     |  GPIO27       |
  | 

  Prototype documentation: https://pu2clr.github.io/RDA5807/
  PU2CLR RDA5807 API documentation: https://pu2clr.github.io/RDA5807/extras/apidoc/html/

  By PU2CLR, Ricardo,  Feb  2023.
*/


#include <RDA5807.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>


#include "Rotary.h"

// LCD 16x02 or LCD20x4 PINs
#define LCD_D7 15
#define LCD_D6 16
#define LCD_D5 17
#define LCD_D4 18
#define LCD_RS 19
#define LCD_E 23


#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF

// Enconder PINs
#define ENCODER_PIN_A 13
#define ENCODER_PIN_B 14
#define RDS_FLAG 34

// Buttons controllers
#define VOLUME_UP 32      // Volume Up
#define VOLUME_DOWN 33    // Volume Down
#define SWITCH_RDS 25     // SDR ON/OFF
#define SWITCH_STEREO 26  // Stereo ON/OFF
#define SEEK_FUNCTION 27  // Seek function

#define POLLING_TIME 1900
#define RDS_MSG_TYPE_TIME 23000
#define POLLING_RDS 51

#define STORE_TIME 10000  // Time of inactivity to make the current receiver status writable (10s / 10000 milliseconds).
#define PUSH_MIN_DELAY 300

#define EEPROM_SIZE 512

const uint8_t app_id = 43;  // Useful to check the EEPROM content before processing useful data
const int eeprom_address = 0;
long storeTime = millis();


bool bSt = true;
bool bRds = true;
bool bShow = false;
uint8_t seekDirection = 1;  // 0 = Down; 1 = Up. This value is set by the last encoder direction.

long pollin_elapsed = millis();

int maxX1;
int maxY1;

// Encoder control variables
volatile int encoderCount = 0;
volatile int rdsCount = 0;
uint16_t currentFrequency;
uint16_t previousFrequency;

// Encoder control
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

// LCD display
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

RDA5807 rx;

void setup() {

  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);

  // Push button pin
  pinMode(VOLUME_UP, INPUT_PULLUP);
  pinMode(VOLUME_DOWN, INPUT_PULLUP);
  pinMode(SWITCH_STEREO, INPUT_PULLUP);
  pinMode(SWITCH_RDS, INPUT_PULLUP);
  pinMode(SEEK_FUNCTION, INPUT_PULLUP);

  // Start LCD display device
  lcd.begin(16, 2);
  showSplash();

  EEPROM.begin(EEPROM_SIZE);

  // If you want to reset the eeprom, keep the ENCODER PUSH BUTTON  pressed during statup
  if (digitalRead(SEEK_FUNCTION) == LOW) {
    EEPROM.write(eeprom_address, 0);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RESET");
    delay(1500);
    showSplash();
  }

  // Encoder interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RDS_FLAG), rdsData, CHANGE);

  rx.setup();

  rx.setInterruptMode(1);  // Sets interrupt on GPIO2 to deal with RDS.

  delay(100);

  // Checking the EEPROM content
  if (EEPROM.read(eeprom_address) == app_id) {
    readAllReceiverInformation();
  } else {
    // Default values
    rx.setVolume(6);
    rx.setMono(false);  // Force stereo
    // rx.setRBDS(true);  //  set RDS and RBDS. See setRDS.
    rx.setRDS(true);
    rx.setRdsFifo(true);
    currentFrequency = previousFrequency = 10390;
  }
  rx.setAFC(true);
  rx.setFrequency(currentFrequency);  // It is the frequency you want to select in MHz multiplied by 100.
  rx.setSeekThreshold(50);            // Sets RSSI Seek Threshold (0 to 127)
  showStatus();
}


void saveAllReceiverInformation() {
  EEPROM.begin(EEPROM_SIZE);

  // The write function/method writes data only if the current data is not equal to the stored data.
  EEPROM.write(eeprom_address, app_id);
  EEPROM.write(eeprom_address + 1, rx.getVolume());           // stores the current Volume
  EEPROM.write(eeprom_address + 2, currentFrequency >> 8);    // stores the current Frequency HIGH byte for the band
  EEPROM.write(eeprom_address + 3, currentFrequency & 0xFF);  // stores the current Frequency LOW byte for the band
  EEPROM.write(eeprom_address + 4, (uint8_t)bRds);
  EEPROM.write(eeprom_address + 5, (uint8_t)bSt);

  EEPROM.end();
}

void readAllReceiverInformation() {
  rx.setVolume(EEPROM.read(eeprom_address + 1));
  currentFrequency = EEPROM.read(eeprom_address + 2) << 8;
  currentFrequency |= EEPROM.read(eeprom_address + 3);
  previousFrequency = currentFrequency;

  bRds = (bool)EEPROM.read(eeprom_address + 4);
  rx.setRDS(bRds);
  rx.setRdsFifo(bRds);

  bSt = (bool)EEPROM.read(eeprom_address + 5);
  rx.setMono(bSt);
}


/*
   To store any change into the EEPROM, it is needed at least STORE_TIME  milliseconds of inactivity.
*/
void resetEepromDelay() {
  delay(PUSH_MIN_DELAY);
  storeTime = millis();
  previousFrequency = 0;
}


/*
  Will be called by the system when a RDS DATA is available
  When the RDA5807FP, pin 16 (GPIO2) has changed 
*/
void rdsData() {
  rdsCount++;
}

/*
    Reads encoder via interrupt
    Use Rotary.h and  Rotary.cpp implementation to process encoder via interrupt
*/
void rotaryEncoder() {  // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
    encoderCount = (encoderStatus == DIR_CW) ? 1 : -1;
}

void showSplash() {
  lcd.setCursor(0, 0);
  lcd.print("PU2CLR-RDA5807");
  lcd.setCursor(0, 1);
  lcd.print("Arduino Library");
  lcd.display();
  delay(1000);
}

/*
   Shows the static content on  display
*/
void showTemplate() {
}


/*
   Shows frequency information on Display
*/
void showFrequency() {
  currentFrequency = rx.getFrequency();
  lcd.setCursor(4, 1);
  lcd.print(rx.formatCurrentFrequency());
  lcd.display();
}

void showFrequencySeek() {
  lcd.clear();
  showFrequency();
}

/*
    Show some basic information on display
*/
void showStatus() {
  lcd.clear();
  showFrequency();
  showStereoMono();
  showRSSI();

  if (bRds) {
    showRds();
  }

  lcd.display();
}

/* *******************************
   Shows RSSI status
*/
void showRSSI() {
  char rssi[12];
  rx.convertToChar(rx.getRssi(), rssi, 3, 0, '.');
  strcat(rssi, "dB");
  lcd.setCursor(13, 1);
  lcd.print(rssi);
}

void showStereoMono() {
  lcd.setCursor(0, 1);
  if (bSt) {
    lcd.print("ST");
  } else {
    lcd.print("MO");
  }
}

/*********************************************************
   RDS
 *********************************************************/
char *programInfo;
char *stationName;
char *stationInfo;
char *rdsTime;
int currentMsgType = 0;
long polling_rds = millis();
long timeTextType = millis();  // controls the type of each text will be shown (Message, Station Name or time)

int progInfoIndex = 0;  // controls the part of the rdsMsg text will be shown on LCD 16x2 Display


/**
  showRdsProgramInfo - Shows the Program Information
*/
void showRdsProgramInfo() {
  char txtAux[17];

  if (programInfo == NULL) return;

  programInfo[61] = '\0';  // Truncate the message to fit on display line
  strncpy(txtAux, &programInfo[progInfoIndex], 16);
  txtAux[16] = '\0';
  progInfoIndex += 3;
  if (progInfoIndex > 60) progInfoIndex = 0;
  lcd.setCursor(0, 0);
  lcd.print(txtAux);
}

/**
   showRDSStation - Shows the 
*/
void showRDSStation() {
  if (stationName == NULL) return;
  lcd.setCursor(0, 0);
  lcd.print(stationName);
}

void showRDSTime() {
  char txtAux[17];

  if (rdsTime == NULL) return;

  rdsTime[16] = '\0';
  strncpy(txtAux, rdsTime, 16);
  txtAux[16] = '\0';
  lcd.setCursor(0, 0);
  lcd.print(txtAux);
}


void clearRds() {
  bShow = false;
  programInfo = NULL;
  stationName = NULL;
  rdsTime = NULL;
  progInfoIndex = currentMsgType = 0;
  rx.clearRdsBuffer();
}

void checkRDS() {
  // You must call getRdsReady before calling any RDS query function.
  if (rx.getRdsReady()) {
    if (rx.hasRdsInfoAB()) {
      programInfo = rx.getRdsProgramInformation();
      stationName = rx.getRdsStationName();
      rdsTime = rx.getRdsTime();
      showRds();
    }
  }
}

void showRds() {

  lcd.setCursor(2, 1);
  if (bRds)
    lcd.print(".");
  else
    lcd.print(" ");

  //if (currentMsgType == 0)
    showRdsProgramInfo();
  //else if (currentMsgType == 1)
  //  showRDSStation();
  //else if (currentMsgType == 2)
  //  showRDSTime();
}

/*********************************************************

 *********************************************************/


void doStereo() {
  rx.setMono((bSt = !bSt));
  bShow = true;
  showStereoMono();
  resetEepromDelay();
}

void doRds() {
  rx.setRDS((bRds = !bRds));
  progInfoIndex = currentMsgType = 0;
  showRds();
  resetEepromDelay();
}

/**
   Process seek command.
   The seek direction is based on the last encoder direction rotation.
*/
void doSeek() {
  rx.seek(RDA_SEEK_WRAP, seekDirection, showFrequencySeek);  // showFrequency will be called by the seek function during the process.
  delay(200);
  bShow = true;
  showStatus();
}

void loop() {

  // Check if the encoder has moved.
  if (encoderCount != 0) {
    if (encoderCount == 1) {
      rx.setFrequencyUp();
      seekDirection = RDA_SEEK_UP;
    } else {
      rx.setFrequencyDown();
      seekDirection = RDA_SEEK_DOWN;
    }
    showStatus();
    bShow = true;
    encoderCount = 0;
    storeTime = millis();
  }

  if (digitalRead(VOLUME_UP) == LOW) {
    rx.setVolumeUp();
    resetEepromDelay();
  } else if (digitalRead(VOLUME_DOWN) == LOW) {
    rx.setVolumeDown();
    resetEepromDelay();
  } else if (digitalRead(SWITCH_STEREO) == LOW)
    doStereo();
  else if (digitalRead(SWITCH_RDS) == LOW)
    doRds();
  else if (digitalRead(SEEK_FUNCTION) == LOW)
    doSeek();

  if ((millis() - pollin_elapsed) > POLLING_TIME) {
    showStatus();
    // if (bShow) clearRds();
    pollin_elapsed = millis();
  }

  //if (bRds) {
    if ((millis() - polling_rds) > POLLING_RDS) {
    // if (rdsCount > 0) {
      //checkRDS();
      rx.getRdsAllData(&stationName, &stationInfo, &programInfo, &rdsTime );
      showRdsProgramInfo();      
      polling_rds = millis();
      rdsCount = 0;
    }
  // }

  if ((millis() - timeTextType) > RDS_MSG_TYPE_TIME) {
    // Toggles the type of message to be shown - See showRds function
    currentMsgType++;
    progInfoIndex = 0;
    if (currentMsgType > 2) currentMsgType = 0;
    timeTextType = millis();
  }

  // Show the current frequency only if it has changed
  if ((currentFrequency = rx.getFrequency()) != previousFrequency) {
    clearRds();
    if ((millis() - storeTime) > STORE_TIME) {
      saveAllReceiverInformation();
      storeTime = millis();
      previousFrequency = currentFrequency;
    }
  }

  delay(5);
}
