#define Master 0
#define Slave 1
#define RX 0
#define TX 1
#define Connected 1
#define Not_connected 0

#define Sw_bt 36
#define ADC_IN 39
#define Vol_P 14
#define Vol_M 27

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

#include <WiFi.h>
#include <WiFiAP.h>
#include <KT0803L.h>
#include <RDA5807_esp.h>
#include <EEPROM.h>

char RX_Message[64];
char TX_Message[64];
char i = 0;

char FW_Version[] = {"TP_1.0.11.23 Tally - Phone"};

char Battery = 100;
char Eter = 0;
char WifiVersion[] = {"esp32_WiFi"};
char Connection = Not_connected;

String Access_point;
String Key;

char Transmitter[] = {"KT0803"};
char Receiver[] = {"RDA5807"};

char WorkMode = Master;
char RX_module = 1;
char TX_module = 1;
char WiFi_module = 0;
char Tally = 1;
char ButtonHold = 0;

byte Volume = 15;

char Address = 255;
char ID[] = {"AB"};

RDA5807 rx; 

unsigned int fm_freq_Tx = 10620;
unsigned int fm_freq_Rx = 10620;
int RSSI_val = 0;
char ConnectMode = 0;

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

void EEPROM_AP_read()
{
  
}

void EE_Erase_sector(int StartAddr,int StopAddr)
{
  for(int i = StartAddr; i<StopAddr; i++)
  {
    EEPROM.write(i ,0);
  }
  EEPROM.commit();
}

void EEPROM_Arr_write(char Arr[], int EE_addr, int St_addr, int Stp_addr, int cnt)
{
  char cnt_ = 0;
  for(int i = St_addr; i<Stp_addr; i++)
  {
     EEPROM.write(EE_addr + cnt_ ,Arr[i]);
     cnt_++;
  }
  EEPROM.commit();
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
  
  fm_freq_Tx = EEPROM.read(3);
  fm_freq_Tx = fm_freq_Tx << 8;
  fm_freq_Tx = fm_freq_Tx | EEPROM.read(4);
  fm_freq_Tx = fm_freq_Tx << 8;
  fm_freq_Tx = fm_freq_Tx | EEPROM.read(5);
  fm_freq_Tx = fm_freq_Tx << 8;
  fm_freq_Tx = fm_freq_Tx | EEPROM.read(6);
  
  fm_freq_Rx = EEPROM.read(7);
  fm_freq_Rx = fm_freq_Rx << 8;
  fm_freq_Rx = fm_freq_Rx | EEPROM.read(8);
  fm_freq_Rx = fm_freq_Rx << 8;
  fm_freq_Rx = fm_freq_Rx | EEPROM.read(9);
  fm_freq_Rx = fm_freq_Rx << 8;
  fm_freq_Rx = fm_freq_Rx | EEPROM.read(10);
  
  WorkMode = EEPROM.read(11);
  RX_module = EEPROM.read(12);
  TX_module = EEPROM.read(13);
  WiFi_module = EEPROM.read(14);
  Tally = EEPROM.read(15);

  EEPROM_Arr_read(WifiVersion, 16, 20);
  EEPROM_Arr_read(Access_point, 36, 40);
  EEPROM_Arr_read(Key, 76, 40);

  EEPROM_AP_read();
  EEPROM_KY_read();

  ButtonHold = EEPROM.read(117);
  //118 - free
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

unsigned int GetNumFromStr(char Arr[], byte k)
{
  char cnt = CharCnt(Arr);
  String str = "";
  unsigned int rez = 0;

  for(int i = 3; i<cnt; i++)
  {
    str += Arr[i];
  }
  rez = str.toInt();

  if(k == 1)
  {
     if(rez >= 5000 && rez <= 11500)
     { return rez; }
  }
  else
  {
    if(rez >= 0 && rez <= 255)
     { return rez; }
  }

  return 0;
}

void COM_Port_Commands()
{
  if(COM_Read() == 1)
  { 
     HAL_Delay(10);
     if(RX_Message[0] == 'F' && RX_Message[1] == 'W')
     { 
        //COM_Write(FW_Version,26);
        Serial.print("TP_1.0.11.23 Tally - Phone"); 
     }

     if(RX_Message[0] == 'F' && RX_Message[1] == 'R')
     {
        digitalWrite(PWR_Red, HIGH);
        digitalWrite(TX_Red, HIGH);
        digitalWrite(WiFi_Red, HIGH);
        digitalWrite(Tally1_Red, HIGH);
        digitalWrite(Tally2_Red, HIGH);
        HAL_Delay(100);
        EEPROM_ERASE();
        TX_Message[0] = 'F';
        TX_Message[1] = 'R';
        TX_Message[2] = '!';
        COM_Write(TX_Message, 3);
        HAL_Delay(1500);
        digitalWrite(TX_Red, LOW);
        digitalWrite(WiFi_Red, LOW);
        digitalWrite(Tally1_Red, LOW);
        digitalWrite(Tally2_Red, LOW);
     }

     if(RX_Message[0] == 'A' && RX_Message[1] == 'P')
     { 
        Serial.print("Access point:"); 
        Serial.print(Access_point); 
     }

     if(RX_Message[0] == 'K' && RX_Message[1] == 'Y')
     {  
        Serial.print("Key:"); 
        Serial.print(Key);
     }
     
     if(RX_Message[0] == 'G' && RX_Message[1] == 'P')
     {
        HAL_Delay(100);
        TX_Clear();
        TX_Message[0] = 'D';
        TX_Message[1] = 'V';
        TX_Message[2] = 'M';
        
        if(WorkMode == 0)
        { TX_Message[3] = '0'; }
        
        if(WorkMode == 1)
        { TX_Message[3] = '1'; }
        
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

        if(Eter == 0)
        { TX_Message[3] = '0'; }
        
        if(Eter == 1)
        { TX_Message[3] = '1'; }
        
        COM_Write(TX_Message, 4);
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'W';
        TX_Message[1] = 'F';
        TX_Message[2] = 'V';
        COM_Write(TX_Message, 3);
        //COM_Write(WifiVersion, CharCnt(WifiVersion));
        Serial.print("ESP32_WiFi");
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'W';
        TX_Message[1] = 'F';
        TX_Message[2] = 'C';

        if(Connection == 0)
        { TX_Message[3] = '0'; }
        
        if(Connection == 1)
        { TX_Message[3] = '1'; }
        
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
        TX_Message[1] = 'H';
        TX_Message[2] = 'T';
        COM_Write(TX_Message, 3);
        Serial.print((float) fm_freq_Tx / 100);
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'F';
        TX_Message[1] = 'H';
        TX_Message[2] = 'R';
        COM_Write(TX_Message, 3);
        Serial.print((float) fm_freq_Rx / 100);
        //COM_Write(Frequency_Rx, CharCnt(Frequency_Rx));
        HAL_Delay(100);

        TX_Clear();
        TX_Message[0] = 'C';
        TX_Message[1] = 'N';
        TX_Message[2] = 'F';
        
        TX_Message[7] = Address;

        if(RX_module == 0)
        { TX_Message[3] = '0'; }
        
        if(RX_module == 1)
        { TX_Message[3] = '1'; }


        if(TX_module == 0)
        { TX_Message[4] = '0'; }
        
        if(TX_module == 1)
        { TX_Message[4] = '1'; }


        if(WiFi_module == 0)
        { TX_Message[5] = '0'; }
        
        if(WiFi_module == 1)
        { TX_Message[5] = '1'; }


        if(Tally == 0)
        { TX_Message[6] = '0'; }
        
        if(Tally == 1)
        { TX_Message[6] = '1'; }
        
        TX_Message[8] = ID[0];
        TX_Message[9] = ID[1];

        if(ButtonHold == 0)
        { TX_Message[10] = '0'; }
        
        if(ButtonHold == 1)
        { TX_Message[10] = '1'; }
        
        COM_Write(TX_Message, 11);
        HAL_Delay(100);
     }

     //Запись параметров
     if(RX_Message[0] == 'D' && RX_Message[1] == 'V' && RX_Message[2] == 'M')
     {
        TX_Clear();

        if(RX_Message[3] == '0' || RX_Message[3] == '1')
        {
           if(RX_Message[3] == '0')
           { WorkMode = Master; EEPROM.write(11, 0); EEPROM.commit();}
           else
           { WorkMode = Slave; EEPROM.write(11, 1); EEPROM.commit();}
           
           TX_Message[0] = 'D';
           TX_Message[1] = 'V';
           TX_Message[2] = 'M';
           TX_Message[3] = ' ';
           TX_Message[4] = 'O';
           TX_Message[5] = 'K';
           COM_Write(TX_Message, 6);
           HAL_Delay(100);
        }
        else
        {
           TX_Message[0] = 'D';
           TX_Message[1] = 'V';
           TX_Message[2] = 'M';
           TX_Message[3] = ' ';
           TX_Message[4] = 'E';
           TX_Message[5] = 'R';
           COM_Write(TX_Message, 6);
           HAL_Delay(100);
        }
     }

     if(RX_Message[0] == 'W' && RX_Message[1] == 'F' && RX_Message[2] == 'A')
     {
        char RxCnt = CharCnt(RX_Message);        
        if(RxCnt > 3)
        {
           EE_Erase_sector(36, 36+40);
           EEPROM_Arr_write(RX_Message, 36, 3, RxCnt);
           TX_Clear();
           TX_Message[0] = 'W';
           TX_Message[1] = 'F';
           TX_Message[2] = 'A';
           TX_Message[3] = ' ';
           TX_Message[4] = 'O';
           TX_Message[5] = 'K';
           COM_Write(TX_Message, 6);
        }
        else
        {
           TX_Clear();
           TX_Message[0] = 'W';
           TX_Message[1] = 'F';
           TX_Message[2] = 'A';
           TX_Message[3] = ' ';
           TX_Message[4] = 'E';
           TX_Message[5] = 'R';
           COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'W' && RX_Message[1] == 'F' && RX_Message[2] == 'K')
     {
        char RxCnt = CharCnt(RX_Message);        
        if(RxCnt > 3)
        {
           EE_Erase_sector(76, 76+40);
           EEPROM_Arr_write(RX_Message, 76, 3, RxCnt);
           TX_Clear();
           TX_Message[0] = 'W';
           TX_Message[1] = 'F';
           TX_Message[2] = 'K';
           TX_Message[3] = ' ';
           TX_Message[4] = 'O';
           TX_Message[5] = 'K';
           COM_Write(TX_Message, 6);
        }
        else
        {
           TX_Clear();
           TX_Message[0] = 'W';
           TX_Message[1] = 'F';
           TX_Message[2] = 'K';
           TX_Message[3] = ' ';
           TX_Message[4] = 'E';
           TX_Message[5] = 'R';
           COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'F' && RX_Message[1] == 'H' && RX_Message[2] == 'T')
     {
        unsigned int F = GetNumFromStr(RX_Message, 1);

        if(F != 0)
        {
           fm_freq_Tx = F;
           EEPROM.write(3, F >> 24 & 0xff);
           EEPROM.write(4, F >> 16 & 0xff);
           EEPROM.write(5, F >> 8 & 0xff);
           EEPROM.write(6, F & 0xff);
           EEPROM.commit();
  
           if(TX_module == 1)
           {
             fmtx_init(fm_freq_Tx, EUROPE);
             delay(10);
             Eter_State(0);
           } 

           TX_Clear();
           TX_Message[0] = 'F';
           TX_Message[1] = 'H';
           TX_Message[2] = 'T';
           TX_Message[3] = ' ';
           TX_Message[4] = 'O';
           TX_Message[5] = 'K';
           COM_Write(TX_Message, 6);
        }
        else
        {
           TX_Clear();
           TX_Message[0] = 'F';
           TX_Message[1] = 'H';
           TX_Message[2] = 'T';
           TX_Message[3] = ' ';
           TX_Message[4] = 'E';
           TX_Message[5] = 'R';
           COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'F' && RX_Message[1] == 'H' && RX_Message[2] == 'R')
     {
        unsigned int F = GetNumFromStr(RX_Message, 1);
        if(F != 0)
        {
           fm_freq_Rx = F;
           EEPROM.write(7, (F >> 24) & 0xff);
           EEPROM.write(8, (F >> 16) & 0xff);
           EEPROM.write(9, (F >> 8) & 0xff);
           EEPROM.write(10, F & 0xff);
           EEPROM.commit();
           if(RX_module == 1)
           {
              rx.setFrequency(fm_freq_Rx);
           }           
           TX_Clear();
           TX_Message[0] = 'F';
           TX_Message[1] = 'H';
           TX_Message[2] = 'R';
           TX_Message[3] = ' ';
           TX_Message[4] = 'O';
           TX_Message[5] = 'K';
           COM_Write(TX_Message, 6);
        }
        else
        {
           TX_Clear();
           TX_Message[0] = 'F';
           TX_Message[1] = 'H';
           TX_Message[2] = 'R';
           TX_Message[3] = ' ';
           TX_Message[4] = 'E';
           TX_Message[5] = 'R';
           COM_Write(TX_Message, 6);
        }
     }
     
     if(RX_Message[0] == 'A' && RX_Message[1] == 'D' && RX_Message[2] == 'R')
     {
        unsigned int F = GetNumFromStr(RX_Message, 0);
        if(F!=0)
        {
          Address = F;
          EEPROM.write(0, Address);
          EEPROM.commit();
          TX_Message[0] = 'A';
          TX_Message[1] = 'D';
          TX_Message[2] = 'R';
          TX_Message[3] = ' ';
          TX_Message[4] = 'O';
          TX_Message[5] = 'K';
          COM_Write(TX_Message, 6);
        }
        else
        {
           TX_Message[0] = 'A';
           TX_Message[1] = 'D';
           TX_Message[2] = 'R';
           TX_Message[3] = ' ';
           TX_Message[4] = 'E';
           TX_Message[5] = 'R';
           COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'D' && RX_Message[1] == 'I' && RX_Message[2] == 'D')
     {
        int k = CharCnt(RX_Message);
        if(k == 5)
        {
          EEPROM.write(1, RX_Message[3]);
          EEPROM.write(2, RX_Message[4]);
          EEPROM.commit();
          ID[0] = RX_Message[3];
          ID[1] = RX_Message[4];
          TX_Message[0] = 'D';
          TX_Message[1] = 'I';
          TX_Message[2] = 'D';
          TX_Message[3] = ' ';
          TX_Message[4] = 'O';
          TX_Message[5] = 'K';
          COM_Write(TX_Message, 6);
        }
        else
        {
          TX_Message[0] = 'D';
          TX_Message[1] = 'I';
          TX_Message[2] = 'D';
          TX_Message[3] = ' ';
          TX_Message[4] = 'E';
          TX_Message[5] = 'R';
          COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'R' && RX_Message[1] == 'X' && RX_Message[2] == 'M')
     {
        if(RX_Message[3] == '1' || RX_Message[3] == '0')
        {
          if(RX_Message[3] == '1')
          { EEPROM.write(12, 1); RX_module = 1;}
          else
          { EEPROM.write(12, 0); RX_module = 0;}
          EEPROM.commit();
          TX_Message[0] = 'R';
          TX_Message[1] = 'X';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'O';
          TX_Message[5] = 'K';
          COM_Write(TX_Message, 6);
        }
        else
        {
          TX_Message[0] = 'R';
          TX_Message[1] = 'X';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'E';
          TX_Message[5] = 'R';
          COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'T' && RX_Message[1] == 'X' && RX_Message[2] == 'M')
     {
        if(RX_Message[3] == '1' || RX_Message[3] == '0')
        {
          if(RX_Message[3] == '1')
          { EEPROM.write(13, 1); TX_module = 1;}
          else
          { EEPROM.write(13, 0); TX_module = 0;}
          EEPROM.commit();
          TX_Message[0] = 'T';
          TX_Message[1] = 'X';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'O';
          TX_Message[5] = 'K';
          COM_Write(TX_Message, 6);
        }
        else
        {
          TX_Message[0] = 'T';
          TX_Message[1] = 'X';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'E';
          TX_Message[5] = 'R';
          COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'T' && RX_Message[1] == 'L' && RX_Message[2] == 'M')
     {
        if(RX_Message[3] == '1' || RX_Message[3] == '0')
        {
          if(RX_Message[3] == '1')
          { EEPROM.write(15, 1); Tally = 1;}
          else
          { EEPROM.write(15, 0); Tally = 0;}
          EEPROM.commit();
          TX_Message[0] = 'T';
          TX_Message[1] = 'L';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'O';
          TX_Message[5] = 'K';
          COM_Write(TX_Message, 6);
        }
        else
        {
          TX_Message[0] = 'T';
          TX_Message[1] = 'L';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'E';
          TX_Message[5] = 'R';
          COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'W' && RX_Message[1] == 'F' && RX_Message[2] == 'M')
     {
        if(RX_Message[3] == '1' || RX_Message[3] == '0')
        {
          if(RX_Message[3] == '1')
          { EEPROM.write(14, 1); }
          else
          { EEPROM.write(14, 0); }
          EEPROM.commit();
          TX_Message[0] = 'W';
          TX_Message[1] = 'F';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'O';
          TX_Message[5] = 'K';
          COM_Write(TX_Message, 6);
        }
        else
        {
          TX_Message[0] = 'W';
          TX_Message[1] = 'F';
          TX_Message[2] = 'M';
          TX_Message[3] = ' ';
          TX_Message[4] = 'E';
          TX_Message[5] = 'R';
          COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'L' && RX_Message[1] == 'B' && RX_Message[2] == 'T')
     {
        if(RX_Message[3] == '1' || RX_Message[3] == '0')
        {
          if(RX_Message[3] == '1')
          { EEPROM.write(117, 1); ButtonHold = 1;}
          else
          { EEPROM.write(117, 0); ButtonHold = 0;}
          EEPROM.commit();
          TX_Message[0] = 'L';
          TX_Message[1] = 'B';
          TX_Message[2] = 'T';
          TX_Message[3] = ' ';
          TX_Message[4] = 'O';
          TX_Message[5] = 'K';
          COM_Write(TX_Message, 6);
        }
        else
        {
          TX_Message[0] = 'L';
          TX_Message[1] = 'B';
          TX_Message[2] = 'T';
          TX_Message[3] = ' ';
          TX_Message[4] = 'E';
          TX_Message[5] = 'R';
          COM_Write(TX_Message, 6);
        }
     }

     if(RX_Message[0] == 'R' && RX_Message[1] == 'B' && RX_Message[2] == 'T')
     {
        Serial.print("Reboot...");
        ESP.restart();
     }
     
     RX_Clear();
  }
}

void CLR(char Arr[], int k)
{
  for(i = 0; i<k; i++)
  { Arr[i] = 0; }
}

void setup() {
  char WiFi_Connect_CNT = 0;
  Serial.begin(115200);
  EEPROM.begin(1024);
  Serial.println("Starting system...");
  Serial.println("Reading settings from EEPROM");
  EEPROM_Read_Settings();
  Serial.println("Reading settings from EEPROM ... Ok");

  Serial.println("Config. HW");
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

  digitalWrite(Vol_P, HIGH);
  digitalWrite(Vol_M, HIGH);
  digitalWrite(Sw_bt, HIGH);
  Serial.println("Config. HW... Ok");

  Serial.println("Config. I2C");
  Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);
  delay(50);
  Serial.println("Config. I2C... Ok");

  Serial.println("Check red LED");  
  digitalWrite(PWR_Red, HIGH);
  digitalWrite(TX_Red, HIGH);
  digitalWrite(WiFi_Red, HIGH);
  digitalWrite(Tally1_Red, HIGH);
  digitalWrite(Tally2_Red, HIGH);

  delay(1000);

  Serial.println("Check green LED");  
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

  Serial.println("Check red + green LED");  
  digitalWrite(PWR_Red, HIGH);
  digitalWrite(TX_Red, HIGH);
  digitalWrite(WiFi_Red, HIGH);
  digitalWrite(Tally1_Red, HIGH);
  digitalWrite(Tally2_Red, HIGH);

  delay(1000);

  Serial.println("Check LED... Ok");

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

  if(digitalRead(Sw_bt) == 0)
  {
    Serial.println("Start in diagnoastic mode");
    Serial.println("Starting WiFI AP");
    digitalWrite(WiFi_Red, HIGH);
    if (!WiFi.softAP("TellyConfig", "AdminConfigAP")) 
    {
      Serial.print("AP error");
      while(1);
    }
    Serial.println("WiFi AP... Ok");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    ConnectMode = 1;

    digitalWrite(WiFi_Red, LOW);
    digitalWrite(WiFi_Green, HIGH);
  }
  else
  {
    Serial.println("Start in normal mode");
    if(RX_module == 1)
    {
      Serial.println("Starting radio receiver");
      digitalWrite(TX_Green, HIGH);
      rx.setup();
      rx.setFrequency(fm_freq_Rx);
      rx.setVolume(Volume);
      rx.setMono(0);
      Serial.println("Radio receiver... Ok");
    }
    else
    {
      Serial.println("Radio receiver disabled");
    }
  
    if(TX_module == 1)
    {
      Serial.println("Starting radio transmitter");
      fmtx_init(fm_freq_Tx, EUROPE);
      delay(10);
      Eter_State(0);
      Serial.println("Radio transmitter... Ok");
    } 
    else
    {Serial.println("Radio transmitter disabled");}

    if(WiFi_module == 1)
    {
      Serial.println("Starting WiFi");
      Serial.print("Connecting to ");
      Serial.println(Access_point);
      digitalWrite(WiFi_Red, HIGH);
      
      WiFi.begin(Access_point, Key);
      //WiFi.begin("StarNet - victor.m03", "485754434306BFAF");

      while (WiFi.status() != WL_CONNECTED && WiFi_Connect_CNT < 50) 
      {
        digitalWrite(WiFi_Green, LOW);
        digitalWrite(WiFi_Red, HIGH);
        delay(250);
        Serial.print(".");
        digitalWrite(WiFi_Green, HIGH);
        digitalWrite(WiFi_Red, LOW);
        delay(250);
        WiFi_Connect_CNT++;
      }

      Serial.println();
      
      if(WiFi_Connect_CNT < 50)
      {
        Serial.println("Starting WiFi... Ok");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        digitalWrite(WiFi_Green, HIGH);
        digitalWrite(WiFi_Red, LOW);
      }
      else
      { 
        Serial.println("WiFi not connected!");
        digitalWrite(WiFi_Green, LOW);
        digitalWrite(WiFi_Red, HIGH); 
      }
    }
    else
    {Serial.println("WiFi disabled");}
    
    Serial.println("System OK");
  }
}

void loop() {

  if(ConnectMode == 1)
  {

  }
  else
  {
     COM_Port_Commands(); 
     if(digitalRead(Vol_P) == 0)
     {
       Volume++;
       if(Volume > 15)
       Volume = 15;
       // if(RX_module == 1)
       // { rx.setVolume(Volume); 
       // delay(100);
     }  
   
     if(digitalRead(Vol_M) == 0)
     {
       if(Volume > 0)
       Volume--;
       // if(RX_module == 1)
       // { rx.setVolume(Volume); 
       // delay(100);
     }

     if(RX_module == 1)
     {
       RSSI_val = rx.getRssi();
       if(RSSI_val > 50)
       { 
          rx.setMute(false);
          digitalWrite(TX_Green, HIGH); 
       }
       else
       { 
          rx.setMute(true); 
          digitalWrite(TX_Green, LOW);
       }
    
       delay(20);
     }
     
  
     if(WorkMode == Master)
     {
        if(fm_freq_Tx != fm_freq_Rx)
        {
           if(digitalRead(Sw_bt) == 0)
           {
              if(Eter == 0)
              {
                if(Get_Eter_State() == 0)
                {
                  digitalWrite(TX_Red, HIGH);
                  Eter_State(1);
                }
                else
                {
                  digitalWrite(TX_Red, LOW);
                  Eter_State(0);
                }
                Eter = 1;
              }
           }
           else
           {
              if(Eter == 1)
              {
                Eter = 0;
              }
           }
        }
        else
        {
           if(digitalRead(Sw_bt) == 0)
           {
              if(Eter == 0 && ButtonHold != 1)
              {
                digitalWrite(TX_Red, HIGH);
                Eter_State(1);
                Eter = 1;
              }

              if(Eter == 0 && ButtonHold == 1)
              {
                if(Get_Eter_State() == 0)
                {
                  digitalWrite(TX_Red, HIGH);
                  Eter_State(1);
                }
                else
                {
                  digitalWrite(TX_Red, LOW);
                  Eter_State(0);
                }

                Eter = 1;
              }
           }
           else
           {
              if(Eter == 1 && ButtonHold != 1)
              {
                digitalWrite(TX_Red, LOW);
                Eter_State(0);
                Eter = 0;
              }

              if(Eter == 1 && ButtonHold == 1)
              {
                Eter = 0;
              }
           }
        }
     }
     else
     {
        if(digitalRead(Sw_bt) == 0)
        {
           if(Eter == 0)
           {
             digitalWrite(TX_Red, HIGH);
             Eter_State(1);
             Eter = 1;
           }
        }
        else
        {
          if(Eter == 1)
          {
            digitalWrite(TX_Red, LOW);
            Eter_State(0);
            Eter = 0;
          }
        }
     }
  }
}
