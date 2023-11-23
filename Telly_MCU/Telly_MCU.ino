#define Master 0
#define Slave 1
#define RX 0
#define TX 1
#define Connected 1
#define Not_connected 0


char RX_Message[64];
char TX_Message[64];

char FW_Version[] = {"TP_1.0.11.23 Tally - Phone"};

char Battery = 100;
char Eter = RX;
char WifiVersion[] = {"----Not equipped----"};
char Connection = Not_connected;

char Access_point[] = {"                                       "};
char Key[] = {"                                       "};

char Transmitter[] = {"KT0803    "};
char Receiver[] = {"RDA5807    "};
char Frequency[] = {"106.0"};

char WorkMode = Master;
char RX_module = 1;
char TX_module = 1;
char WiFi_module = 0;
char Tally = 1;

char Address = 255;
char ID[] = {"AB"};


void EEPROM_Read(char Message)
{
  
}
void HAL_Delay(unsigned int n)
{
  delay(n);
}
void RX_Clear()
{
  char i = 0;
  for(i=0;i<64;i++)
  {
    RX_Message[i] = 0;
  }
}
void TX_Clear()
{
  char i = 0;
  for(i=0;i<64;i++)
  {
    TX_Message[i] = 0;
  }
}
char COM_Read()
{
  char i = 0;
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
  char i;
   for(i = 0; i<CharNum; i++)
   {
      Serial.write(Message[i]);
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
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
}

void loop() {
  COM_Port_Commands(); 
}
