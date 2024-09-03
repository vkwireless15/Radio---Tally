/*****************************************************************************/
/*!
    @file     	FMTX.cpp
    @author   	www.elechouse.com
	@version  	V1.0
	@date		2012-11-1
	@brief    	FMTX demo header file

	@section  HISTORY
	
    V1.0    Initial version.

    Copyright (c) 2012 www.elechouse.com  All right reserved.
*/
/*****************************************************************************/

#include "KT0803L.h"
#include <Wire.h>

// I2C bus pin on ESP32
#define ESP32_I2C_SDA 21
#define ESP32_I2C_SCL 22

char EterSt = 0;

void i2c_init(void)
{
   Wire.begin(ESP32_I2C_SDA, ESP32_I2C_SCL);
}
void i2c_start(byte Address)
{
   //Wire.beginTransmission(Address);
}
void i2c_stop(void)
{
   //Wire.endTransmission();
}

void i2c_write(byte dt)
{
   //Wire.write(dt);
}

byte i2c_read_ack(void)
{

}

byte i2c_read_nack(void)
{
   //return Wire.read();
}

byte fmtx_reg[18]={
    0x00, 0x01, 0x02, 0x04, 0x0B,
    0x0C, 0x0E, 0x0F, 0x10, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17,
    0x1E, 0x26, 0x27,
};

byte fmtx_reg_val[18]={

};

void fmtx_write_reg(byte reg, byte dt)
{	
	byte dat[] = {reg, dt};
	Wire.beginTransmission(FMTX_CMD_WRITE);
	Wire.write(dat, 2);
	Wire.endTransmission(true); 
}

byte fmtx_read_reg(byte reg)
{
    byte dt;	
	Wire.beginTransmission(FMTX_CMD_WRITE);
	Wire.write(reg);
	Wire.endTransmission(false);
	
	Wire.requestFrom(FMTX_CMD_READ, 1); // reading 0x0A register
    delayMicroseconds(250);
    dt = Wire.read();
	Wire.endTransmission(true);
	
    return dt;
}

/**
    Register    0x00, 0x01, 0x02, 0x04, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x1E, 0x26, 0x27,
    Read        0x5C, 0xC3, 0x40, 0x04, 0x00, 0x00, 0xC2, 0xFB, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xA0, 0x00,
    Default     0x5C, 0xC3, 0x40, 0x04, 0x00, 0x00, 0x02, 0x14, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xA0, 0x00,
    others      0x7A, 0x03, 0x01, 0x04, 0x00, 0x00, 0xC2, 0xFB, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x04, 0x40, 0xA0, 0x00,
				0x5C, 0xC3, 0x40, 0x04, 0x00, 0x00, 0xC2, 0x00, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x00, 0x40, 0xA0, 0x00,
*/
void fmtx_read_all(byte *buf)
{
    byte i;
    for(i=0; i<18; i++){
        buf[i] = fmtx_read_reg(fmtx_reg[i]);
    }
}

void fmtx_set_freq(unsigned long freq)
{
    unsigned int f;
    byte reg0, reg1_8, reg9_11;

    reg0 = fmtx_read_reg(0x02);
    reg9_11 = fmtx_read_reg(0x01);

    freq /= 5;
    f = (unsigned int)freq;
    f &= 0x0FFF;

    if(f&0x01){
        reg0 |= 0x80;
    }else{
        reg0 &= ~0x80;
    }

    reg1_8 = (byte)(f>>1);
    reg9_11 = (reg9_11&0xF8) | (byte)(f>>9);

    fmtx_write_reg(0x02, reg0);
    fmtx_write_reg(0x01, reg9_11);
    fmtx_write_reg(0x00, reg1_8);

}

void fmtx_set_pga(fmtx_pga_type pga)
{
    byte reg;
    byte pga_val;
    reg = fmtx_read_reg(0x01);
    pga_val = (byte)pga;
    pga_val &= ~0xC7;
    reg = (reg&0xC7) | pga_val;
    fmtx_write_reg(0x01, reg);
}

void fmtx_set_rfgain(byte rfgain)
{
    byte reg3, reg0_1, reg2;

    reg0_1 = fmtx_read_reg(0x01);
    reg2 = fmtx_read_reg(0x13);
    reg3 = fmtx_read_reg(0x02);

    rfgain &= 0x0F;
    reg0_1 = (reg0_1&0x3F) | (rfgain<<6);
    if(rfgain & 0x04){
        reg2 |= 0x80;
    }else{
        reg2 &= ~0x80;
    }

    if(rfgain & 0x08){
        reg3 |= 0x40;
    }else{
        reg3 &= ~0x40;
    }

    fmtx_write_reg(0x01, reg0_1);
    fmtx_write_reg(0x13, reg2);
    fmtx_write_reg(0x02, reg3);
}

void fmtx_set_alc(byte sta)
{
    byte reg;
    reg = fmtx_read_reg(0x04);
    if(!sta){
        reg &= ~0x80;
    }else{
        reg |= 0x80;
    }
    fmtx_write_reg(0x04, reg);
}

void fmtx_pa_external()
{
    byte reg;

    reg = fmtx_read_reg(0x13);
    while( !(fmtx_read_reg(0x0F)&0x10));

    reg |= 0x04;
    fmtx_write_reg(0x13, reg);
}

void fmtx_set_sl(void)
{
    byte reg;
    reg = fmtx_read_reg(0x12);
    fmtx_write_reg(0x12, (reg&0x81) | 0x7E);

    reg = fmtx_read_reg(0x14);
    fmtx_write_reg(0x14, (reg&0x02) | 0xDD);

    reg = fmtx_read_reg(0x16);
    fmtx_write_reg(0x16, (reg&0xF8) | 0x07);

    reg = fmtx_read_reg(0x0B);
    fmtx_write_reg(0x1B, reg | 0x04);

    reg = fmtx_read_reg(0x12);
    fmtx_write_reg(0x12, reg&0x7F);
}

void fmtx_set_phcnst(country_type country)
{
    byte reg;
    reg = fmtx_read_reg(0x02);
    switch(country){
        case USA:
        case JAPAN:
            reg &= ~0x01;
            break;
        case EUROPE:
        case AUSTRALIA:
        case CHINA:
            reg |= 0x01;
            break;
        default:
            break;
    }
    fmtx_write_reg(0x02, reg);
}

void fmtx_set_au_enhance(void)
{
    byte reg;
    reg = fmtx_read_reg(0x17);

    fmtx_write_reg(0x17, reg |= 0x20);
}

void fmtx_set_xtal(void)
{
    byte reg;
    reg = fmtx_read_reg(0x1E);
    fmtx_write_reg(0x1E, reg | 0x40);
}

void fmtx_init(unsigned long freq, country_type country)
{
	i2c_init();
	fmtx_set_freq(freq);
	fmtx_set_rfgain(4);
	fmtx_set_pga(PGA_0DB);
	fmtx_set_phcnst(country);
	fmtx_set_xtal();
}


void Eter_State(unsigned char state)
{
   byte reg;
   reg = fmtx_read_reg(0x0B);
   if(state == 1)
   {
      fmtx_write_reg(0x0B, reg &= 0x7F);
	  EterSt = 1;
   }
   else
   {
      fmtx_write_reg(0x0B, reg |= 0x80);
	  EterSt = 0;
   }
}

char Get_Eter_State()
{
   return EterSt;
}
