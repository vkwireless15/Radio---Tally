#define Master 0
#define Slave 1
#define RX 0
#define TX 1
#define Connected 1
#define Not_connected 0
#define Disabled 2
#define Ok 1
#define Error 0

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

//#define Enabled 1
//#define Not_Connected 0

#define ESP32_I2C_SDA 21
#define ESP32_I2C_SCL 22

#include <WiFi.h>
#include <WiFiAP.h>
#include <KT0803L.h>
#include <RDA5807_esp.h>
#include <EEPROM.h>
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
#include <Arduino_JSON.h>
#include <Preferences.h>
#include <nvs_flash.h>

SocketIOclient socket;
RDA5807 rx; 
Preferences Mem_Settings;

JSONVar BusOptions;
JSONVar Devices;
JSONVar DeviceStates;

String DeviceId = "";
String listenerDeviceName = "";

String ServerIP = "";
String ServerPort = "";

String DevIP = "";
String GatewayIP = "";
String SubnetIP = "";

String actualType = "";
String prevType = "";
String actualColor = "";
int actualPriority = 0;

bool serverConnected = false;

char RX_Message[64];
char TX_Message[64];
char rx_count = 0;

String FW_Version = "TP_1.0.11.23 Tally - Phone";

char Battery = 100;
char Eter = 0;
String WifiVersion = "esp32_WiFi";

String Access_point;
String Key;

String Transmitter = "KT0803";
String Receiver = "RDA5807";

char WorkMode = Master;
char RX_module = 1;
char TX_module = 1;
char WiFi_module = 0;
char Tally = 1;
char ButtonHold = 0;
char DHCP = 0;
int RSSI_Tr = 0;
char RSSI_Tr_act = 0;
char RadioRxIsAct = 0;
char HoldFlag = 0;

byte Volume = 15;
byte Mem_Volume = 15;

unsigned int fm_freq_Tx = 0;
unsigned int fm_freq_Rx = 0;
int RSSI_val = 0;
char ConnectMode = 0;

char Wifi_st = Disabled;
char Power_st = 0;

//WebServer server(3333);

void EEPROM_Read_Settings()
{
  Mem_Settings.begin("SettingsEEPROM", false);

  WorkMode = Mem_Settings.getChar("DVM", 1);
  RX_module = Mem_Settings.getChar("RXM", 0);
  TX_module = Mem_Settings.getChar("TXM", 0);
  Tally = Mem_Settings.getChar("TLM", 0);
  ButtonHold = Mem_Settings.getChar("LBT", 0);
  WiFi_module = Mem_Settings.getChar("WFM", 0);
  fm_freq_Tx = Mem_Settings.getUInt("FHT", 9000);
  fm_freq_Rx = Mem_Settings.getUInt("FHR", 9000);
  Access_point = Mem_Settings.getString("WFA","AP");
  Key = Mem_Settings.getString("WFK","12345678");
  DHCP = Mem_Settings.getChar("DHC", 1);
  ServerPort = Mem_Settings.getString("PRT","4455");
  ServerIP = Mem_Settings.getString("SIP","192.168.4.2");
  DevIP = Mem_Settings.getString("DIP","192.168.4.1");
  DeviceId = Mem_Settings.getString("DID","empty");
  listenerDeviceName = Mem_Settings.getString("DNM","RT");
  RSSI_Tr = Mem_Settings.getUInt("SSA",20);
  RSSI_Tr_act = Mem_Settings.getChar("RTE",1);
  Volume = Mem_Settings.getUInt("VOL",20);

  GatewayIP = Mem_Settings.getString("DGA","0.0.0.0");
  SubnetIP = Mem_Settings.getString("DSA","255.255.255.0");

  Mem_Settings.end();
}

void HAL_Delay(unsigned int n)
{
  delay(n);
}

void RX_Clear()
{
  char j = 0;
  rx_count = 0;
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
    RX_Message[rx_count] = Serial.read();
    rx_count++;
  }
  if(rx_count>0)
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
     if(rez >= 8750 && rez <= 10800)
     { return rez; }
  }
  if(k == 0)
  {
    if(rez >= 0 && rez <= 255)
     { return rez; }
  }
  if(k == 2)
  {
    if(rez >= 1 && rez <= 255)
     { return rez; }
  }

  return 0;
}

void COM_Port_Commands()
{
  if(COM_Read() == 1)
  { 
     String AP;
     HAL_Delay(10);
     if(RX_Message[0] == 'F' && RX_Message[1] == 'W')
     { 
         Serial.println(FW_Version); 
     }

     if(RX_Message[0] == 'S' && RX_Message[1] == 'R')
     { 
       for(unsigned int k = 8750; k <= 10800; k+= 10)
       {
         rx.setFrequency(k);
         HAL_Delay(500);
         RSSI_val = rx.getRssi();
         Serial.print(String((float) k / 100));
         Serial.print(" ");
         Serial.println(RSSI_val);
       }
        HAL_Delay(500);
        rx.setFrequency(fm_freq_Tx);
        Serial.println("SR OK");
     }

     if(RX_Message[0] == 'F' && RX_Message[1] == 'R')
     {
        digitalWrite(PWR_Red, HIGH);
        digitalWrite(TX_Red, HIGH);
        digitalWrite(WiFi_Red, HIGH);
        digitalWrite(Tally1_Red, HIGH);
        digitalWrite(Tally2_Red, HIGH);
        HAL_Delay(100);
        
        nvs_flash_erase(); 
        nvs_flash_init();
        
        Serial.print("FR ok!"); 
        HAL_Delay(1500);
        digitalWrite(TX_Red, LOW);
        digitalWrite(WiFi_Red, LOW);
        digitalWrite(Tally1_Red, LOW);
        digitalWrite(Tally2_Red, LOW);
     }

     if(RX_Message[0] == 'G' && RX_Message[1] == 'R')
     {
       RSSI_val = rx.getRssi();
       Serial.print("RSSI: "); Serial.println(RSSI_val); 
     }
     
     if(RX_Message[0] == 'G' && RX_Message[1] == 'P')
     {
        HAL_Delay(100);

        Serial.println("DVM" + String((byte)WorkMode));
        HAL_Delay(100);

        Serial.println("TLM"+ String((byte)Tally));
        HAL_Delay(100);

        Serial.println("LBT"+ String((byte)ButtonHold));
        HAL_Delay(100);

        Serial.println("RXM"+ String((byte)RX_module));
        HAL_Delay(100);

        Serial.println("TXM"+ String((byte)TX_module));
        HAL_Delay(100);
        
        Serial.println("FMT" + Transmitter);
        HAL_Delay(100);

        Serial.println("FMR" + Receiver);
        HAL_Delay(100);

        Serial.println("FHT" + String((float) fm_freq_Tx / 100));
        HAL_Delay(100);

        Serial.println("FHR" + String((float) fm_freq_Rx / 100));
        HAL_Delay(100);

        Serial.println("WFM"+ String((byte)WiFi_module));
        HAL_Delay(100);
        
        Serial.println("WFV" + WifiVersion);
        HAL_Delay(100);

        Serial.println("WFA" + Access_point);
        HAL_Delay(30);

        Serial.println("WFK" + Key);
        HAL_Delay(100);

        Serial.println("DHC"+ String((byte)DHCP));
        HAL_Delay(100);

        Serial.println("DIP" + DevIP);
        HAL_Delay(100);

        Serial.println("SIP" + ServerIP);
        HAL_Delay(100);

        Serial.println("PRT" + ServerPort);
        HAL_Delay(100);

        Serial.println("DID" + DeviceId);
        HAL_Delay(100);

        Serial.println("DNM" + listenerDeviceName);
        HAL_Delay(100);

     }

     //Запись параметров
     if(RX_Message[0] == 'V' && RX_Message[1] == 'O' && RX_Message[2] == 'L')
     {
        unsigned int F = GetNumFromStr(RX_Message, 0);
        char RxCnt = CharCnt(RX_Message);  
        if(RxCnt == 3)
        {
          Serial.println("VOL" + String(Volume));
        }
        else
        {
          if(F < 16)
          {
            Volume = F;
            Mem_Settings.begin("SettingsEEPROM", false);
            Mem_Settings.putUInt("VOL", Volume);
            Mem_Settings.end();
          
            Serial.println("VOL OK");
           }
           else
           {
            Serial.println("VOL ER");
           }
         }
     }
     
     if(RX_Message[0] == 'D' && RX_Message[1] == 'V' && RX_Message[2] == 'M')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("DVM" + String(WorkMode));
        }
        else
        {
          if(RX_Message[3] == '0' || RX_Message[3] == '1')
          {
            Mem_Settings.begin("SettingsEEPROM", false);
           
            if(RX_Message[3] == '0')
            { Mem_Settings.putChar("DVM", 0); }
            else
            { Mem_Settings.putChar("DVM", 1); }
           
            Mem_Settings.end();
            Serial.println("DVM OK");
            HAL_Delay(100);
          }
          else
          {
            Serial.println("DVM ER");
            HAL_Delay(100);
          }
        }
     }

     if(RX_Message[0] == 'W' && RX_Message[1] == 'F' && RX_Message[2] == 'A')
     {
        char RxCnt = CharCnt(RX_Message); 
        if(RxCnt == 3)
        {
          Serial.println("WFA" + Access_point);
        }
        else
        {       
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
            Mem_Settings.begin("SettingsEEPROM", false);
            for(int i = 3; i<RxCnt; i++)
            {
              if(RX_Message[i] >= 0x20)
              { AP+= RX_Message[i]; }
            }
            Mem_Settings.putString("WFA", AP);
            Serial.println("WFA OK");
            Mem_Settings.end();
          }
          else
          {
            Serial.println("WFA ER");
          }
        }
     }

     if(RX_Message[0] == 'W' && RX_Message[1] == 'F' && RX_Message[2] == 'K')
     {
        char RxCnt = CharCnt(RX_Message);  
        if(RxCnt == 3)
        {
          Serial.println("WFK" + Key);
        }
        else
        {      
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
            Mem_Settings.begin("SettingsEEPROM", false);
            for(int i = 3; i<RxCnt; i++)
            {
              if(RX_Message[i] >= 0x20)
              { AP+= RX_Message[i]; }
            }
            Mem_Settings.putString("WFK", AP);
            Serial.println("WFK OK");
            Mem_Settings.end();
          }
          else
          {
            Serial.println("WFK ER");
          }
        }
     }

     if(RX_Message[0] == 'F' && RX_Message[1] == 'H' && RX_Message[2] == 'T')
     {
        unsigned int F = GetNumFromStr(RX_Message, 1);
        char RxCnt = CharCnt(RX_Message);  
        if(RxCnt == 3)
        {
          Serial.println("FHT" + String(fm_freq_Tx));
        }
        else
        {
          if(F != 0)
          {
            fm_freq_Tx = F;
            Mem_Settings.begin("SettingsEEPROM", false);
            Mem_Settings.putUInt("FHT", fm_freq_Tx);
            Mem_Settings.end();
           
            if(TX_module == 1)
            {
              fmtx_init(fm_freq_Tx, EUROPE);
              delay(10);
              Eter_State(0);
            } 
            Serial.println("FHT OK");
           }
           else
           {
            Serial.println("FHT ER");
           }
         }
     }

     if(RX_Message[0] == 'F' && RX_Message[1] == 'H' && RX_Message[2] == 'R')
     {
        unsigned int F = GetNumFromStr(RX_Message, 1);
        char RxCnt = CharCnt(RX_Message);  
        if(RxCnt == 3)
        {
          Serial.println("FHR" + String(fm_freq_Rx));
        }
        else
        {
          if(F != 0)
          {
            fm_freq_Rx = F;
            Mem_Settings.begin("SettingsEEPROM", false);
            Mem_Settings.putUInt("FHR", fm_freq_Rx);
            Mem_Settings.end();

            if(RX_module == 1)
            {
              rx.setFrequency(fm_freq_Rx);
            }           
            Serial.println("FHR OK");
          }
          else
          {
            Serial.println("FHR ER");
          }
        }
     }

     if(RX_Message[0] == 'S' && RX_Message[1] == 'S' && RX_Message[2] == 'A')
     {
        unsigned int F = GetNumFromStr(RX_Message, 2);
        char RxCnt = CharCnt(RX_Message);  
        if(RxCnt == 3)
        {
          Serial.println("SSA" + String(RSSI_val));
        }
        else
        {
          if(F != 0)
          {
            RSSI_Tr = F;
            Mem_Settings.begin("SettingsEEPROM", false);
            Mem_Settings.putUInt("SSA", F);
            Mem_Settings.end();          
            Serial.println("SSA OK");
          }
          else
          {
            Serial.println("SSA ER");
          }
        }
     }

     if(RX_Message[0] == 'R' && RX_Message[1] == 'T' && RX_Message[2] == 'E')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("RTE" + String(RSSI_Tr_act));
        }
        else
        {
          if(RX_Message[3] == '0' || RX_Message[3] == '1')
          {
            Mem_Settings.begin("SettingsEEPROM", false);
           
            if(RX_Message[3] == '0')
            { Mem_Settings.putChar("RTE", 0); RSSI_Tr_act = 0; }
            else
            { Mem_Settings.putChar("RTE", 1); RSSI_Tr_act = 1; }
           
            Mem_Settings.end();
            Serial.println("RTE OK");
            HAL_Delay(100);
          }
          else
          {
            Serial.println("RTE ER");
            HAL_Delay(100);
          }
        }
     }
     
     if(RX_Message[0] == 'D' && RX_Message[1] == 'I' && RX_Message[2] == 'D')
     {
        char RxCnt = CharCnt(RX_Message);    
        if(RxCnt == 3)
        {
          Serial.println("DID" + DeviceId);
        }
        else
        {     
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
            Mem_Settings.begin("SettingsEEPROM", false);
            for(int i = 3; i<RxCnt; i++)
            {
              if(RX_Message[i] >= 0x20)
              { AP+= RX_Message[i]; }
            }
            Mem_Settings.putString("DID", AP);
            Serial.println("DID OK");
            Mem_Settings.end();
          }
          else
          {
            Serial.println("DID ER");
          }
        }
     }

     if(RX_Message[0] == 'R' && RX_Message[1] == 'X' && RX_Message[2] == 'M')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("RXM" + String(RX_module));
        }
        else
        {
          if(RX_Message[3] == '1' || RX_Message[3] == '0')
          {
            Mem_Settings.begin("SettingsEEPROM", false);
            if(RX_Message[3] == '1')
            { Mem_Settings.putChar("RXM", 1); }
            else
            { Mem_Settings.putChar("RXM", 0); }
          
            Mem_Settings.end();
            Serial.println("RXM OK");
          }
          else
          {
            Serial.println("RXM ER");
          }
        }
     }

     if(RX_Message[0] == 'T' && RX_Message[1] == 'X' && RX_Message[2] == 'M')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("TXM" + String(TX_module));
        }
        else
        {
          if(RX_Message[3] == '1' || RX_Message[3] == '0')
          {
             Mem_Settings.begin("SettingsEEPROM", false);
           
             if(RX_Message[3] == '0')
             { Mem_Settings.putChar("TXM", 0); }
             else
             { Mem_Settings.putChar("TXM", 1); }
           
             Mem_Settings.end();
             Serial.println("TXM OK");
             HAL_Delay(100);
          }
          else
          {
            Serial.println("TXM ER");
          }
        }
     }

     if(RX_Message[0] == 'T' && RX_Message[1] == 'L' && RX_Message[2] == 'M')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("TLM" + String(Tally));
        }
        else
        {
          if(RX_Message[3] == '1' || RX_Message[3] == '0')
          {
            Mem_Settings.begin("SettingsEEPROM", false);
           
            if(RX_Message[3] == '0')
            { Mem_Settings.putChar("TLM", 0); }
            else
            { Mem_Settings.putChar("TLM", 1); }
           
            Mem_Settings.end();
            Serial.println("TLM OK");
            HAL_Delay(100);
          }
          else
          {
            Serial.println("TLM ER");
          }
        }
     }

     if(RX_Message[0] == 'W' && RX_Message[1] == 'F' && RX_Message[2] == 'M')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("WFM" + String(WiFi_module));
        }
        else
        {
          if(RX_Message[3] == '1' || RX_Message[3] == '0')
          {
            Mem_Settings.begin("SettingsEEPROM", false);
           
            if(RX_Message[3] == '0')
            { Mem_Settings.putChar("WFM", 0); }
            else
            { Mem_Settings.putChar("WFM", 1); }
           
            Mem_Settings.end();
            Serial.println("WFM OK");
            HAL_Delay(100);
          }
          else
          {
            Serial.println("WFM ER");
          }
        }
     }

     if(RX_Message[0] == 'L' && RX_Message[1] == 'B' && RX_Message[2] == 'T')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("LBT" + String(ButtonHold));
        }
        else
        {
          if(RX_Message[3] == '1' || RX_Message[3] == '0')
          {
            Mem_Settings.begin("SettingsEEPROM", false);
           
            if(RX_Message[3] == '0')
            { Mem_Settings.putChar("LBT", 0); }
            else
            { Mem_Settings.putChar("LBT", 1); }
           
            Mem_Settings.end();
            Serial.println("LBT OK");
            HAL_Delay(100);
          }
          else
          {
            Serial.println("LBT ER");
          }
        }
     }

     if(RX_Message[0] == 'D' && RX_Message[1] == 'H' && RX_Message[2] == 'C')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("DHC" + String(DHCP));
        }
        else
        {
          if(RX_Message[3] == '0' || RX_Message[3] == '1')
          {
            Mem_Settings.begin("SettingsEEPROM", false);
           
            if(RX_Message[3] == '0')
            { Mem_Settings.putChar("DHC", 0); }
            else
            { Mem_Settings.putChar("DHC", 1); }
           
            Mem_Settings.end();
            Serial.println("DHC OK");
            HAL_Delay(100);
          }
          else
          {
            Serial.println("DHC ER");
            HAL_Delay(100);
          }
        }
     }

     if(RX_Message[0] == 'S' && RX_Message[1] == 'I' && RX_Message[2] == 'P')
     {
        char RxCnt = CharCnt(RX_Message);
        if(RxCnt == 3)
        {
          Serial.println("SIP" + ServerIP);
        }
        else
        {        
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
             Mem_Settings.begin("SettingsEEPROM", false);
             for(int i = 3; i<RxCnt; i++)
             {
               if(RX_Message[i] >= 0x20)
               { AP+= RX_Message[i]; }
             }
             Mem_Settings.putString("SIP", AP);
             Serial.println("SIP OK");
             Mem_Settings.end();
           }
           else
           {
             Serial.println("SIP ER");
           }
        }
     }

     if(RX_Message[0] == 'D' && RX_Message[1] == 'I' && RX_Message[2] == 'P')
     {
        char RxCnt = CharCnt(RX_Message); 
        if(RxCnt == 3)
        {
          Serial.println("DIP" + DevIP);
        }
        else
        {        
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
             Mem_Settings.begin("SettingsEEPROM", false);
             for(int i = 3; i<RxCnt; i++)
             {
               if(RX_Message[i] >= 0x20)
               { AP+= RX_Message[i]; }
             }
             Mem_Settings.putString("DIP", AP);
             Serial.println("DIP OK");
             Mem_Settings.end();
          }
          else
          {
             Serial.println("DIP ER");
          }
        }
     }

     if(RX_Message[0] == 'D' && RX_Message[1] == 'G' && RX_Message[2] == 'A')
     {
        char RxCnt = CharCnt(RX_Message); 
        if(RxCnt == 3)
        {
          Serial.println("DGA" + GatewayIP);
        }
        else
        {         
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
             Mem_Settings.begin("SettingsEEPROM", false);
             for(int i = 3; i<RxCnt; i++)
             {
                if(RX_Message[i] >= 0x20)
                { AP+= RX_Message[i]; }
             }
             Mem_Settings.putString("DGA", AP);
             Serial.println("DGA OK");
             Mem_Settings.end();
          }
          else
          {
             Serial.println("DGA ER");
          }
        }
     }

     if(RX_Message[0] == 'D' && RX_Message[1] == 'S' && RX_Message[2] == 'A')
     {
        char RxCnt = CharCnt(RX_Message); 
        if(RxCnt == 3)
        {
          Serial.println("DSA" + SubnetIP);
        }
        else
        {         
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
             Mem_Settings.begin("SettingsEEPROM", false);
             for(int i = 3; i<RxCnt; i++)
             {
                if(RX_Message[i] >= 0x20)
                { AP+= RX_Message[i]; }
             }
             Mem_Settings.putString("DSA", AP);
             Serial.println("DSA OK");
             Mem_Settings.end();
          }
          else
          {
             Serial.println("DSA ER");
          }
        }
     }

     if(RX_Message[0] == 'P' && RX_Message[1] == 'R' && RX_Message[2] == 'T')
     {
        char RxCnt = CharCnt(RX_Message); 
        if(RxCnt == 3)
        {
          Serial.println("PRT" + ServerPort);
        }
        else
        {         
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
             Mem_Settings.begin("SettingsEEPROM", false);
             for(int i = 3; i<RxCnt; i++)
             {
               if(RX_Message[i] >= 0x20)
               { AP+= RX_Message[i]; }
             }
             Mem_Settings.putString("PRT", AP);
             Serial.println("PRT OK");
             Mem_Settings.end();
          }
          else
          {
             Serial.println("PRT ER");
          }
        }
     }

     if(RX_Message[0] == 'D' && RX_Message[1] == 'N' && RX_Message[2] == 'M')
     {
        char RxCnt = CharCnt(RX_Message); 
        if(RxCnt == 3)
        {
          Serial.println("DNM" + listenerDeviceName);
        }
        else
        {         
          if(RxCnt > 3 && RxCnt - 3 < 40)
          {
            Mem_Settings.begin("SettingsEEPROM", false);
            for(int i = 3; i<RxCnt; i++)
            {
              if(RX_Message[i] >= 0x20)
              { AP+= RX_Message[i]; }
            }
            Mem_Settings.putString("DNM", AP);
            Serial.println("DNM OK");
            Mem_Settings.end();
          }
          else
          {
            Serial.println("DNM ER");
          }
        }
     }
     
     if(RX_Message[0] == 'R' && RX_Message[1] == 'B' && RX_Message[2] == 'T')
     {
        Serial.println("Reboot...");
        ESP.restart();
     }
     
     RX_Clear();
  }
}

void CLR(char Arr[], int k)
{
  for(char i = 0; i<k; i++)
  { Arr[i] = 0; }
}


void ws_emit(String event, const char *payload = NULL) {
  if (payload) {
    String msg = "[\"" + event + "\"," + payload + "]";
    //Serial.println(msg);
    socket.sendEVENT(msg);
  } else {
    String msg = "[\"" + event + "\"]";
    //Serial.println(msg);
    socket.sendEVENT(msg);
  }
}

void socket_Connected(const char * payload, size_t length) {
  //Serial.println("Connected to Tally Arbiter server.");
  //Serial.println("DeviceId: " + DeviceId);
  String deviceObj = "{\"deviceId\": \"" + DeviceId + "\", \"listenerType\": \"" + listenerDeviceName.c_str() + "\", \"canBeReassigned\": true, \"canBeFlashed\": true, \"supportsChat\": true }";
  char charDeviceObj[1024];
  strcpy(charDeviceObj, deviceObj.c_str());
  ws_emit("listenerclient_connect", charDeviceObj);
}

void evaluateMode() {
  if(actualType != prevType) 
  {
    if (actualType != "") 
    {
      
    } 
    else 
    {
      //Serial.println("No set!");
      digitalWrite(Tally1_Red, LOW);
      digitalWrite(Tally2_Red, LOW);
      digitalWrite(Tally1_Green, LOW);
      digitalWrite(Tally2_Green, LOW);
    }
    
    if (actualType == "\"preview\"") 
    {
      //Serial.println("preview");
      digitalWrite(Tally1_Green, HIGH);
      digitalWrite(Tally2_Green, HIGH);
      digitalWrite(Tally1_Red, LOW);
      digitalWrite(Tally2_Red, LOW);
    } 
    if (actualType == "\"program\"") 
    {
      //Serial.println("program");
      digitalWrite(Tally1_Red, HIGH);
      digitalWrite(Tally2_Red, HIGH);
      digitalWrite(Tally1_Green, LOW);
      digitalWrite(Tally2_Green, LOW);
    }
    
    prevType = actualType;
  }
}

void SetDeviceName() {
  String DeviceName;
  for (int i = 0; i < Devices.length(); i++) {
    if (JSON.stringify(Devices[i]["id"]) == "\"" + DeviceId + "\"") {
      String strDevice = JSON.stringify(Devices[i]["name"]);
      DeviceName = strDevice.substring(1, strDevice.length() - 1);
      break;
    }
  }
  evaluateMode();
}

void socket_Reassign(String payload) {
  String oldDeviceId = payload.substring(0, payload.indexOf(','));
  String newDeviceId = payload.substring(oldDeviceId.length()+1);
  newDeviceId = newDeviceId.substring(0, newDeviceId.indexOf(','));


  String reassignObj = "{\"oldDeviceId\": \"" + oldDeviceId + "\", \"newDeviceId\": \"" + newDeviceId + "\"}";
  char charReassignObj[1024];
  strcpy(charReassignObj, reassignObj.c_str());
  ws_emit("listener_reassign_object", charReassignObj);
  ws_emit("devices");

  //Serial.println("newDeviceId: " + newDeviceId);
  
  newDeviceId.remove(0,1);
  newDeviceId.remove(newDeviceId.length()-1, 1);
  //Serial.println("newDeviceId: " + newDeviceId);

  DeviceId = newDeviceId;
  SetDeviceName();
  
  Mem_Settings.begin("SettingsEEPROM", false);
  Mem_Settings.putString("DID", newDeviceId);
  Mem_Settings.end();
  
}

void socket_Flash()
{
  digitalWrite(Tally1_Green, HIGH);
  digitalWrite(Tally2_Green, HIGH);
  digitalWrite(Tally1_Red, LOW);
  digitalWrite(Tally2_Red, LOW);
  HAL_Delay(1000);
  digitalWrite(Tally1_Green, LOW);
  digitalWrite(Tally2_Green, LOW);
  digitalWrite(Tally1_Red, LOW);
  digitalWrite(Tally2_Red, LOW);
}

void showDeviceInfo() {
  //displays the currently assigned device and tally data
  evaluateMode();
}

String getBusTypeById(String busId) {
  for (int i = 0; i < BusOptions.length(); i++) {
    if (JSON.stringify(BusOptions[i]["id"]) == busId) {
      return JSON.stringify(BusOptions[i]["type"]);
    }
  }

  return "invalid";
}

String getBusColorById(String busId) {
  for (int i = 0; i < BusOptions.length(); i++) {
    if (JSON.stringify(BusOptions[i]["id"]) == busId) {
      return JSON.stringify(BusOptions[i]["color"]);
    }
  }

  return "invalid";
}

int getBusPriorityById(String busId) {
  for (int i = 0; i < BusOptions.length(); i++) {
    if (JSON.stringify(BusOptions[i]["id"]) == busId) {
      return (int) JSON.stringify(BusOptions[i]["priority"]).toInt();
    }
  }

  return 0;
}

void processTallyData() {
  bool typeChanged = false;
  for (int i = 0; i < DeviceStates.length(); i++) {
    if (DeviceStates[i]["sources"].length() > 0) {
      typeChanged = true;
      actualType = getBusTypeById(JSON.stringify(DeviceStates[i]["busId"]));
      actualColor = getBusColorById(JSON.stringify(DeviceStates[i]["busId"]));
      actualPriority = getBusPriorityById(JSON.stringify(DeviceStates[i]["busId"]));
    }
  }
  if(!typeChanged) {
    actualType = "";
    actualColor = "";
    actualPriority = 0;
  }
  evaluateMode();
}

void socket_event(socketIOmessageType_t type, uint8_t * payload, size_t length) { 

    String msg = (char*)payload;
    String type1 = msg.substring(2, msg.indexOf("\"",2));
    String content = msg.substring(type1.length() + 4);
    content.remove(content.length() - 1);
    
  
  switch (type) {
    case sIOtype_CONNECT:
      socket_Connected((char*)payload, length);
      Wifi_st = Connected;
      break;

    case sIOtype_DISCONNECT:
    case sIOtype_ACK:
    case sIOtype_ERROR:
    case sIOtype_BINARY_EVENT:
    case sIOtype_BINARY_ACK:
      // Not handled
      break;

    case sIOtype_EVENT:     
      if (type1 == "bus_options") BusOptions = JSON.parse(content);
      if (type1 == "reassign") socket_Reassign(content);
      if (type1 == "flash") socket_Flash();

      if (type1 == "deviceId") {
        DeviceId = content.substring(1, content.length()-1);
        SetDeviceName();
        showDeviceInfo();
      }

      if (type1 == "devices") {
        Devices = JSON.parse(content);
        SetDeviceName();
      }

      if (type1 == "device_states") {
        DeviceStates = JSON.parse(content);
        //Serial.println("Got event '" + type1 + "', data: " + content);
        processTallyData();
      }

      break;
  }
}

void GetOctIP(String IP_a, int &oct1, int &oct2, int &oct3, int &oct4)
{
  char IP_char[20];
  char oct_cnt = 0;
  String oct = "";
  IP_a.toCharArray(IP_char, IP_a.length()+1);

  //Serial.print(String(IP_a.length()) + " ");

  for(int i = 0; i<IP_a.length(); i++)
  {
    if(IP_char[i] != '.')
    {
      oct += IP_char[i];
      
    }
    else
    {
      //Serial.print(oct + " ");
      
      if(oct_cnt == 0)
      {oct1 = oct.toInt();}
      
      if(oct_cnt == 1)
      {oct2 = oct.toInt();}
      
      if(oct_cnt == 2)
      {oct3 = oct.toInt();}
      
      oct="";
      oct_cnt++;
    }
  }
  oct4 = oct.toInt(); 
  //Serial.print(oct + " "); 
}

void St_Leds()
{
  static int Timer_cnt;

  if(Timer_cnt <= 3)
  {
    if(Wifi_st == Not_connected)
    { 
      digitalWrite(WiFi_Red, HIGH); 
    }
    
    if(Wifi_st == Connected)
    { 
      digitalWrite(WiFi_Green, HIGH); 
    }
    
    if(Wifi_st == Disabled)
    { 
      digitalWrite(WiFi_Red,LOW); 
      digitalWrite(WiFi_Green,LOW); 
    }

    if(Power_st == Ok)
    { 
      digitalWrite(PWR_Red, LOW); 
      digitalWrite(PWR_Green, HIGH); 
    }

    if(Power_st == Error)
    { 
      digitalWrite(PWR_Red, HIGH); 
      digitalWrite(PWR_Green, LOW);
    }
  }
  else
  {
    digitalWrite(PWR_Red,LOW);
    digitalWrite(PWR_Green,LOW);

    digitalWrite(WiFi_Red,LOW);
    digitalWrite(WiFi_Green,LOW);
  }

  Timer_cnt++;
  
  if(Timer_cnt > 100)
  Timer_cnt = 0;
}

void Main_Process()
{
  if(digitalRead(Vol_P) == 0)
     {
       Volume++;
       if(Volume > 15)
       Volume = 15;

       Mem_Settings.begin("SettingsEEPROM", false);
       Mem_Settings.putUInt("VOL", Volume);
       Mem_Settings.end();
       
       delay(100);
     }  
   
     if(digitalRead(Vol_M) == 0)
     {
       if(Volume > 0)
       Volume--;

       Mem_Settings.begin("SettingsEEPROM", false);
       Mem_Settings.putUInt("VOL", Volume);
       Mem_Settings.end();
       
       delay(100);
     }

     if(RX_module == 1)
     {
       if(Volume != Mem_Volume)
       {
         rx.setVolume(Volume);
         Mem_Volume = Volume;
       }
       
       if(RSSI_Tr_act == 1)
       {
         RSSI_val = rx.getRssi();
         if(RSSI_val > RSSI_Tr && Eter == 0 && HoldFlag == 0)
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
       else
       {
         if(RadioRxIsAct == 0)
         {
           rx.setMute(false);
           digitalWrite(TX_Green, HIGH);
           RadioRxIsAct = 1;
         }
         delay(20);
       }
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
                rx.setMute(true); 
                digitalWrite(TX_Green, LOW);
                
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

                  rx.setMute(true); 
                  digitalWrite(TX_Green, LOW);
                }
                else
                {
                  digitalWrite(TX_Red, LOW);
                  Eter_State(0);
                  if(RSSI_Tr_act == 0)
                  {
                    rx.setMute(false); 
                    digitalWrite(TX_Green, HIGH);
                    HoldFlag = 0;
                  }
                }

                Eter = 1;
              }
           }
           else
           {
              if(Eter == 1 && ButtonHold != 1)
              {
                if(RSSI_Tr_act == 0)
                {
                  rx.setMute(false); 
                  digitalWrite(TX_Green, HIGH);
                }
            
                digitalWrite(TX_Red, LOW);
                Eter_State(0);
                Eter = 0;
              }

              if(Eter == 1 && ButtonHold == 1)
              {
                HoldFlag = 1;
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
             if(fm_freq_Tx != fm_freq_Rx)
             {
               digitalWrite(TX_Red, HIGH);
               Eter_State(1);
               Eter = 1;
             }
             else
             {
               rx.setMute(true); 
               digitalWrite(TX_Green, LOW);
               digitalWrite(TX_Red, HIGH);
               Eter_State(1);
               Eter = 1;
             }
           }
        }
        else
        {
          if(Eter == 1)
          {
            if(RSSI_Tr_act == 0)
            {
               rx.setMute(false); 
               digitalWrite(TX_Green, HIGH);
            }
            digitalWrite(TX_Red, LOW);
            Eter_State(0);
            Eter = 0;
          }
        }
     }
}

void setup() {
  char WiFi_Connect_CNT = 0;
  char ServIP[20];
  char ServPort[10];
  int Oct1, Oct2, Oct3, Oct4;
  
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
      Wifi_st = Not_connected;
      
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
        if(DHCP == 0)
        {
          GetOctIP(DevIP, Oct1, Oct2, Oct3, Oct4);
          IPAddress local_IP(Oct1, Oct2, Oct3, Oct4);

          GetOctIP(GatewayIP, Oct1, Oct2, Oct3, Oct4);
          IPAddress gateway(Oct1, Oct2, Oct3, Oct4);

          GetOctIP(SubnetIP, Oct1, Oct2, Oct3, Oct4);
          IPAddress subnet(Oct1, Oct2, Oct3, Oct4);
        }
        else
        {
          DevIP = (String)WiFi.localIP();
          Serial.println(DevIP);
        }
        
        digitalWrite(WiFi_Green, HIGH);
        digitalWrite(WiFi_Red, LOW);

        ServerIP.toCharArray(ServIP, ServerIP.length()+1);
        ServerPort.toCharArray(ServPort, ServerPort.length()+1);
        
        Serial.println("Connecting to Tally Arbiter host: " + String(ServIP) + " Port:" + String(ServPort));
        socket.onEvent(socket_event);
        socket.begin(ServIP, atol(ServPort));
      }
      else
      { 
        Serial.println("WiFi not connected!");
        digitalWrite(WiFi_Green, LOW);
        digitalWrite(WiFi_Red, HIGH); 
      }
    }
    else
    {Serial.println("WiFi disabled"); Wifi_st = Disabled;}
    
    Serial.println("System OK");
    digitalWrite(PWR_Red, LOW);
    digitalWrite(PWR_Green, HIGH);
    Power_st = Ok;
  }
}

void loop() {
  
  static unsigned long LastTick, Stop_time;
  static byte socket_block_flag;

  if(ConnectMode == 1)
  {

  }
  else
  {
     LastTick = millis();
     if(socket_block_flag != 1)
     { socket.loop(); }
     else
     {
       if(millis() > Stop_time)
       socket_block_flag = 0;
       Wifi_st = Not_connected;
     }
     if(millis() > LastTick + 1000)
     {
        socket_block_flag = 1;
        Stop_time = millis() + 10000;
     }
     
     St_Leds();
     COM_Port_Commands();
     Main_Process();
  }
}
