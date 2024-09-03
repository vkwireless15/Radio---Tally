/*****************************************************************************/
/*!
    @file     	FMTX.h
    @author   	www.elechouse.com
	@version  	V1.0
	@date		2012-11-1
	@brief    	FMTX demo header file

	@section  HISTORY
	
    V1.0    Initial version.

    Copyright (c) 2012 www.elechouse.com  All right reserved.
*/
/*****************************************************************************/

#ifndef __KT0803L_H
#define __KT0803L_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "wiring_private.h"

#ifndef __TYPE_REDEFINE
#define __TYPE_REDEFINE
#endif

#ifndef I2C_FREQ
  #define I2C_FREQ 100000L
#endif

#define FMTX_CMD_WRITE      (0x7C >> 1)
#define FMTX_CMD_READ       (0x7D >> 1)



typedef enum{
    PGA_0DB = 4,
    PGA_4DB = 5,
    PGA_8DB = 6,
    PGA_12DB = 7,
    PGA_N0DB = 0,
    PGA_N4DB = 1,
    PGA_N8DB = 2,
    PGA_N12DB = 3,
}fmtx_pga_type;
typedef enum{
    USA,
    JAPAN,
    EUROPE,
    AUSTRALIA,
    CHINA,
}country_type;



// void i2c_init(void);
// void fmtx_write_reg(u8 reg, u8 dt);
// u8 fmtx_read_reg(u8 reg);
// void fmtx_read_all(u8 *buf);
// void fmtx_set_alc(u8 sta);
// void fmtx_set_pga(fmtx_pga_type pga);
// void fmtx_pa_external();
// void fmtx_set_sl(void);
// void fmtx_set_rfgain(u8 rfgain);
// void fmtx_set_phcnst(country_type country);
// void fmtx_set_au_enhance(void);
// void fmtx_set_xtal(void);

void fmtx_init(unsigned long freq, country_type country);
void fmtx_set_freq(unsigned long freq);
void Eter_State(unsigned char state);
char Get_Eter_State();

// extern u8 fmtx_reg_val[18];
// extern u8 fmtx_reg[18];



#endif

