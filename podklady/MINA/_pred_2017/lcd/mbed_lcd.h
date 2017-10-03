#ifndef _MBED_LCD
#define _MBED_LCD

#include "mbed.h"

void MBED_LCD_set_start_line(byte x);
void MBED_LCD_set_page(byte p);
void MBED_LCD_init_all(void);

void MBED_LCD_write_data(byte val);
void MBED_LCD_write_bytes(byte *bptr, int count);

bool MBED_LCD_WriteCharXY(char c, byte col, byte row);
bool MBED_LCD_WriteStringXY(char *cp, byte col, byte row);
void MBED_LCD_FillDisp(byte bFill);

#endif
