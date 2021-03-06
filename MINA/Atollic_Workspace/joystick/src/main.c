/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 8.0.0   2017-10-03

The MIT License (MIT)
Copyright (c) 2009-2017 Atollic AB

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm_core.h"
#include "mina_shield.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  // Joystick Initialization
  Nucleo_SetPinGPIO(JOY_UP, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_DOWN, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_LEFT, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_RIGHT, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_PUSH, ioPortInputFloat);

  // RGB led Initialization
  Nucleo_SetPinGPIO(RGB_RED, ioPortOutputPP);
  Nucleo_SetPinGPIO(RGB_GREEN, ioPortOutputPP);
  Nucleo_SetPinGPIO(RGB_BLUE, ioPortOutputPP);
  /* TODO - Add your application code here */

  unsigned short joy_combination = 0;
  /* Infinite loop */
  while (1)
  {
    joy_combination = GPIORead(JOY_UP)*16 + GPIORead(JOY_DOWN)*8 +
                    GPIORead(JOY_LEFT)*4 + GPIORead(JOY_RIGHT)*2 +
                    GPIORead(JOY_PUSH);

    switch(joy_combination)
    {
      case 0b1:
        GPIOWrite(RGB_RED, 0);
        break;
      case 0b10:
        GPIOWrite(RGB_BLUE, 0);
        break;
      case 0b100:
        GPIOWrite(RGB_GREEN, 0);
        break;
      case 0b1000:
        GPIOWrite(RGB_GREEN, 0);
        GPIOWrite(RGB_RED, 0);
        break;
      case 0b10000:
        GPIOWrite(RGB_BLUE, 0);
        GPIOWrite(RGB_RED, 0);
        break;
      default:
        GPIOWrite(RGB_RED, 1);
        GPIOWrite(RGB_GREEN, 1);
        GPIOWrite(RGB_BLUE, 1);
        break;
    }

  }
}
