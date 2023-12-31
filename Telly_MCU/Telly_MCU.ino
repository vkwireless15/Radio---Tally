#define Master 0
#define Slave 1
#define RX 0
#define TX 1
#define Connected 1
#define Not_connected 0

#define Sw_bt 36
#define ADC_IN 39
#define Vol_P 25
#define Vol_M 26

#define PWR_Red 32
#define PWR_Green 33

#define TX_Red 25
#define TX_Green 26

#define WiFi_Red 4
#define WiFi_Green 16

#define Tally1_Red 17
#define Tally1_Green 5
#define Tally2_Red 19
#define Tally2_Green 18

#define ESP32_I2C_SDA 21
#define ESP32_I2C_SCL 22


#include <KT0803L.h>
#include <RDA5807_esp.h>
#include <EEPROM.h>

char RX_Message[64];
char TX_Message[64];
char i = 0;

char FW_Version[] = {"TP_1.0.11.23 Tally - Phone"};

char Battery = 100;
char Eter = RX;
char WifiVersion[] = {"----Not equipped----"};
char Connection = Not_connected;

char Access_point[] = {"                                        "};
char Key[] = {"                                        "};

char Transmitter[] = {"KT0803    "};
char Receiver[] = {"RDA5807   "};
char Frequency[] = {"106.0"};

char WorkMode = Master;
char RX_module = 1;
char TX_module = 1;
char WiFi_module = 0;
char Tally = 1;

char Address = 255;
char ID[] = {"AB"};

RDA5807 rx; 
float fm_freq = 106;
unsigned long rx_fm_freq = 106000;

float EEPROM_float_read(int addr)
{   
  byte x[4];
  for(byte i = 0; i < 4; i++) x[i] = EEPROM.read(i+addr);
  float *y = (float *)&x;
  return y[0];
}

void EEPROM_float_write(int addr, float val)
{ 
  byte *x = (byte *)&val;
  for(byte i = 0; i < 4; i++) EEPROM.write(i+addr, x[i]);
  EEPROM.commit();
}

void EEPROM_Arr_read(char Arr[], int addr, int cnt)
{
  for(int i = 0; i<cnt; i++)
  {
    Arr[i] = EEPROM.read(addr + i);
  }
}

void EEPROM_ERASE()
{
  for(int k = 0; k<1024; k++)
  { EEPROM.write(k,0); }
  EEPROM.commit();
}

void EEPROM_Read_Settings()
{
  Address = EEPROM.read(0);
  ID[0] = EEPROM.read(1);
  ID[1] = EEPROM.read(2);
  
  fm_freq = EEPROM_float_read(3);
  
  rx_fm_freq = EEPROM.read(7);
  rx_fm_freq << 8;
  rx_fm_freq = rx_fm_freq | EEPROM.read(8);
  rx_fm_freq << 8;
  rx_fm_freq = rx_fm_freq | EEPROM.read(9);
  rx_fm_freq << 8;
  rx_fm_freq = rx_fm_freq | EEPROM.read(10);
  
  WorkMode = EEPROM.read(11);
  RX_module = EEPROM.read(12);
  TX_module = EEPROM.read(13);
  WiFi_module = EEPROM.read(14);
  Tally = EEPROM.read(15);

  EEPROM_Arr_read(WifiVersion, 16, 20);
  EEPROM_Arr_read(Access_point, 36, 40);
  EEPROM_Arr_read(Key, 76, 40);

  EEPROM_Arr_read(Transmitter, 116, 10);
  EEPROM_Arr_read(Receiver, 126, 10);
  EEPROM_Arr_read(Frequency, 136, 5); 

  fm_freq = (float)106.1;
  rx_fm_freq = 106100;
  //141 - free
}
void HAL_Delay(unsigned int n)
{
  delay(n);
}
void RX_Clear()
{
  char j = 0;
  i = 0;
  for(j=0;j<64;j++)
  {
    RX_Message[j] = 0;
  }
}
void TX_Clear()
{
  char j = 0;
  for(j=0;j<64;j++)
  {
    TX_Message[j] = 0;
  }
}
char COM_Read()
{
  RX_Clear();

  if(Serial.available() > 0)
  {delay(30);}
  
  while (Serial.available() > 0)
  {
    RX_Message[i] = Serial.read();
    i++;
  }
  if(i>0)
  { return 1;}
  return 0;
}
void COM_Write(char Message[], char CharNum)
{
  char j;
   for(j = 0; j<CharNum; j++)
   {
      Serial.write(Message[j]);
   }
}
int CharCnt(char mess[])
{
  int CNT = 0;
  while(mess[CNT] != 0)
  {CNT++;}
  return CNT;
}
void COM_Port_Commands()
{
  if(COM_Read() == 1)
  { 
     if(RX_Message[0] == 'F' && RX_Message[1] == 'W')
     { 
        COM_Write(FW_Version,26); 
     }

     if(RX_Message[0] == 'F' && RX_Message[1] == 'R')
     {
        HAL_Delay(100);
        EEPROM_ERASE();
        TX_Message[0] = 'F';
        TX_Message[1] = 'R';
        TX_Message[2] = '!';
     }
     
     if(RX_Message[0] == 'G' && RX_Message[1] == 'P')
     {
        HAL_Delay(100);
        TX_Clear();
        TX_Message[0] = 'D';
        TX_Message[1] = 'V';
        TX_Message[2] = 'M';
        TX_Message[3] = WorkMode;
        COM_Write(TX_Message, 4);
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'D';
        TX_Message[1] = 'V';
        TX_Message[2] = 'B';
        TX_Message[3] = Battery;
        COM_Write(TX_Message, 4);
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'D';
        TX_Message[1] = 'V';
        TX_Message[2] = 'E';
        TX_Message[3] = Eter;
        COM_Write(TX_Message, 4);
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'W';
        TX_Message[1] = 'F';
        TX_Message[2] = 'V';
        COM_Write(TX_Message, 3);
        COM_Write(WifiVersion, CharCnt(WifiVersion));
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'W';
        TX_Message[1] = 'F';
        TX_Message[2] = 'C';
        TX_Message[3] = Connection;
        COM_Write(TX_Message, 4);
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'W';
        TX_Message[1] = 'F';
        TX_Message[2] = 'A';
        COM_Write(TX_Message, 3);
        COM_Write(Access_point, CharCnt(Access_point));
        HAL_Delay(30);

        TX_Clear();
        TX_Message[0] = 'W';
        TX_Message[1] = 'F';
        TX_Message[2] = 'K';
        COM_Write(TX_Message, 3);
        COM_Write(Key, CharCnt(Key));
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'F';
        TX_Message[1] = 'M';
        TX_Message[2] = 'T';
        COM_Write(TX_Message, 3);
        COM_Write(Transmitter, CharCnt(Transmitter));
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'F';
        TX_Message[1] = 'M';
        TX_Message[2] = 'R';
        COM_Write(TX_Message, 3);
        COM_Write(Receiver, CharCnt(Receiver));
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'F';
        TX_Message[1] = 'M';
        TX_Message[2] = 'F';
        COM_Write(TX_Message, 3);
        COM_Write(Frequency, CharCnt(Frequency));
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'C';
        TX_Message[1] = 'N';
        TX_Message[2] = 'F';
        TX_Message[3] = RX_module;
        TX_Message[4] = TX_module;
        TX_Message[5] = WiFi_module;
        TX_Message[6] = Tally;
        TX_Message[7] = Address;
        TX_Message[8] = ID[0];
        TX_Message[9] = ID[1];
        COM_Write(TX_Message, 10);
        HAL_Delay(100);
     }
     RX_Clear();
  }
}

void setup() {
  EEPROM.begin(1024);
  EEPROM_Read_Settings();
  Serial.begin(115200);

  pinMode(Sw_bt, INPUT);
  pinMode(ADC_IN, INPUT);
  pinMode(Vol_P, INPUT);
  pinMode(Vol_M, INPUT);
  
  pinMode(PWR_Red, OUTPUT);
  pinMode(PWR_Green, OUTPUT);
  
  pinMode(TX_Red, OUTPUT);
  pinMode(TX_Green, OUTPUT);
  
  pinMode(WiFi_Red, OUTPUT);
  pinMode(WiFi_Green, OUTPUT);
  
  pinMode(Tally1_Red, OUTPUT);
  pinMode(Tally1_Green, OUTPUT);
  pinMode(Tally2_Red, OUTPUT);
  pinMode(Tally2_Green, OUTPUT);

  Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);
  delay(50);
    
  digitalWrite(PWR_Red, HIGH);
  digitalWrite(TX_Red, HIGH);
  digitalWrite(WiFi_Red, HIGH);
  digitalWrite(Tally1_Red, HIGH);
  digitalWrite(Tally2_Red, HIGH);

  delay(1000);

  digitalWrite(PWR_Red, LOW);
  digitalWrite(TX_Red, LOW);
  digitalWrite(WiFi_Red, LOW);
  digitalWrite(Tally1_Red, LOW);
  digitalWrite(Tally2_Red, LOW);
  
  digitalWrite(PWR_Green, HIGH);
  digitalWrite(TX_Green, HIGH);
  digitalWrite(WiFi_Green, HIGH);
  digitalWrite(Tally1_Green, HIGH);
  digitalWrite(Tally2_Green, HIGH);

  delay(1000);

  digitalWrite(PWR_Red, HIGH);
  digitalWrite(TX_Red, HIGH);
  digitalWrite(WiFi_Red, HIGH);
  digitalWrite(Tally1_Red, HIGH);
  digitalWrite(Tally2_Red, HIGH);

  delay(1000);

  digitalWrite(PWR_Red, HIGH);
  digitalWrite(TX_Red, LOW);
  digitalWrite(WiFi_Red, LOW);
  digitalWrite(Tally1_Red, LOW);
  digitalWrite(Tally2_Red, LOW);

  digitalWrite(PWR_Green, LOW);
  digitalWrite(TX_Green, LOW);
  digitalWrite(WiFi_Green, LOW);
  digitalWrite(Tally1_Green, LOW);
  digitalWrite(Tally2_Green, LOW);

  rx.setup();
  rx.setFrequency(10620);
  rx.setVolume(5);
  rx.setMono(0);

  fmtx_init(106.2, EUROPE); 
  
}

void loop() {
  COM_Port_Commands(); 
  Serial.println(rx.getRssi());
  delay(500);
  
  if(WorkMode == Master)
  {
    
  }
  else
  {
    
  }
}
