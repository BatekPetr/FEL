#include "mbed.h"
#include "uart.h"
#include "systick_ms.h"

#include "mbed_i2c.h"

void printAccValue(uint8_t val)
{
  printf("%02X ", val);
  putchar((val & 0x40) ? 'A' : 'n');  // alarm bit
  putchar(' ');

  {
    int x = 0;
    
    if (val & 0x20)    // signed 6-bit value
      x = - (32 - (val & 0x1f));
    else
      x = val & 0x1f;
   
    // http://developer.mbed.org/users/Sissors/code/MMA7660/file/36a163511e34/MMA7660.h
    #define MMA7660_SENSITIVITY 21.33
    printf("%5d = %8.5f\r\n", x, x / MMA7660_SENSITIVITY);
  }
}

int main(void)
{
  int cnt = 0;
  uint16_t w = 0;
  float t;
  uint8_t x, y, z, st;
  
  InitSystick(1);
  Uart2Init(38400);
  
  SET_IO_LED_B;

  InitI2C();
  
  WriteXYZI2C(0x07, 0x01);    // Mode Registr: 0000 00x1
    // D7  D6  D5   D4  D3  D2  D1 D0
    // IAH IPP SCPS ASE AWE TON -  MODE
  
  while(1)
  {
    TOGGLE_IO(GPIOA, 9);
    cnt++;
    printf("Cyklus %d:\r\n", cnt);

    w = ReadTempI2C();
    printf("Temp: RAW: %04X", w);
        
    w >>= 5;
          
    if (w & (1 << 10))      // D10 == 1 - negative value
      t = (((int)w) - 0x7ff) * 0.125;
    else
      t = w * 0.125;

    printf("  real: %.3f\r\n", t);
       
    ReadXYZI2C(&x, &y, &z, &st);
    printf("ACC RAW: %02X %02X %02X %02X\r\n", x, y, z, st);  // show pure values
        
    printAccValue(x);
    printAccValue(y);
    printAccValue(z);
        
      // show Tilt Status (reg. $03)
    printf("Tilt: Shake=%c, Alert=%c, Tap=%c",
      (st & 0x80) ? '1' : '0', (st & 0x40) ? '1' : '0', (st & 0x20) ? '1' : '0');
    printf(", PoLa=");  // position in landscape mode
    switch((st >> 2) & 0x07)
    {
      case 0: printf("n/a"); break;    // 000
      case 1: printf("left"); break;   // 001
      case 2: printf("right"); break;  // 010
      case 5: printf("down"); break;   // 101
      case 6: printf("up"); break;     // 110
      default: printf("err"); break;
    }
    
    printf(", BaFro=");  // back/front
    switch(st & 0x03)
    {
      case 0: printf("n/a"); break;    // 00
      case 1: printf("front"); break;  // 01
      case 2: printf("back"); break;   // 10
      default: printf("err"); break;
    }
    printf("\r\n");

    cekejMs(500);
  }
}
