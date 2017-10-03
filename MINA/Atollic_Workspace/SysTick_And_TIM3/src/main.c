/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 8.0.0   2017-09-25

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
#include <mina_shield.h>

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
volatile uint32_t _ticks = 0;
void SysTick_Handler(void)
{
_ticks++;
}

int main(void)
{
  uint32_t tm_blue = 0;
  uint32_t tm_green = 0;

  if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN))
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
  }
/*
  TIM3->CR1 = TIM_CR1_DIR; // DIR = 1 = cnt-down, ostatni v 0
  TIM3->CR2 = 0;
  TIM3->PSC = SystemCoreClock / 1000 - 1; // 1ms = 1kHz
  TIM3->ARR = 999; // (100 - 1) - tj. 10Hz, 100ms
  TIM3->CR1 |= TIM_CR1_CEN; // CEN = 1 = enable
*/
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);
  Nucleo_SetPinGPIO(RGB_GREEN, ioPortOutputPP);
//  Nucleo_SetPinGPIO(RGB_RED, ioPortOutputPP);

  while (1)
  {
    if ((_ticks - 500) >= tm_green)
    {
      tm_green = _ticks;
      GPIOToggle(RGB_GREEN);
    }
/*
    if (TIM3->SR & TIM_SR_UIF) // nenula = nastaven bit
    {
      TIM3->SR &= ~TIM_SR_UIF; // shodit priznak !!!
      GPIOToggle(RGB_RED);
    }
*/
  }
}