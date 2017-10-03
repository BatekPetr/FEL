#include "mbed_lcd.h"
#include "mbed_lcd_hw.h"

void MBED_LCD_write_data(byte val)
{
  MBED_LCD_send(val, 1);
}

void MBED_LCD_write_bytes(byte *bptr, int count)
{
  int i;
  for(i = 0; i < count; i++)
    MBED_LCD_send(bptr[i], 1);
}


void MBED_LCD_set_start_line(byte x)
{
  MBED_LCD_send(0x10 | ((x & 0xf0) >> 4), 0);    // (2) Display start line set = Sets the display RAM display start lineaddress - lower 6 bits
  MBED_LCD_send(0x00 | (x & 0x0f), 0);    // (2) Display start line set = Sets the display RAM display start lineaddress - lower 6 bits
}

void MBED_LCD_set_page(byte p)
{
   MBED_LCD_send(0xB0 | (p & 0x0f), 0);   // (3) Page address set = Sets the display RAM page address - lower 4 bits
}

void MBED_LCD_init_all(void)
{
  MBED_LCD_init_hw();
  MBED_LCD_reset();
  
  MBED_LCD_send(0xAE, 0);   //  display off
  MBED_LCD_send(0xA2, 0);   //  bias voltage

  MBED_LCD_send(0xA0, 0);
  MBED_LCD_send(0xC8, 0);   //  colum normal

  MBED_LCD_send(0x22, 0);   //  voltage resistor ratio
  MBED_LCD_send(0x2F, 0);   //  power on
  //wr_cmd(0xA4);   //  LCD display ram
  MBED_LCD_send(0x40, 0);   // start line = 0
  MBED_LCD_send(0xAF, 0);     // display ON

  MBED_LCD_send(0x81, 0);   //  set contrast
  MBED_LCD_send(0x17, 0);   //  set contrast

  MBED_LCD_send(0xA6, 0);     // display normal
//  MBED_LCD_send(0xA7, 0);     // display inverted

//  MBED_LCD_send(0xa5, 0);
}

#include "font_8x8.h"

bool MBED_LCD_WriteCharXY(char c, byte col, byte row)
{
  int i;
  
	if ((col > 128 / 8) || (row > 3))
		return false;

  MBED_LCD_set_page(row);
  MBED_LCD_set_start_line(col * 8);

  if (c > 127)
    c %= 128;
  
  for (i = 0; i < 8; i++)
		MBED_LCD_send(font8x8_basic[c * 8 + i], 1);

	return true;
}

bool MBED_LCD_WriteStringXY(char *cp, byte col, byte row)
{
  int i;

	if ((col > 128 / 8) || (row > 3))
		return false;

  MBED_LCD_set_page(row);
  MBED_LCD_set_start_line(col * 8);

  for (; *cp; cp++)
  {
    dword offset = *cp * 8;
    for ( i = 0; i < 8; i++)
      MBED_LCD_send(font8x8_basic[offset++], 1);
  }
  
  return true;
}

void MBED_LCD_FillDisp(byte bFill)
{
  uint x;
  
  for(x = 0; x < 128 * 32 / 8; x++)
  {
    if ((x % 128) == 0)
    {
      MBED_LCD_set_page(x / 128);
      MBED_LCD_set_start_line(0);
    }

    MBED_LCD_send(bFill, 1);
  }
}
