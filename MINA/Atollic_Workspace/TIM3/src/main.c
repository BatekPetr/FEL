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

volatile bool tim3irq = false;

void TIM3_IRQHandler(void) // obsluha preruseni od TIM3
{
  TIM3->SR &= ~TIM_SR_UIF; // shodit priznak !!
  tim3irq = true;
}

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
  if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN))
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
  }

  TIM3->CR1 = TIM_CR1_DIR; // DIR = 1 = cnt-down, ostatni v 0
  TIM3->CR2 = 0;
  TIM3->PSC = SystemCoreClock / 1000 - 1; // 1ms = 1kHz
  TIM3->ARR = 99; // (100 - 1) - tj. 10Hz, 100ms

  TIM3->DIER = TIM_DIER_UIE;
  TIM3->CR1 |= TIM_CR1_CEN; // CEN = 1 = enable

  NVIC_EnableIRQ(TIM3_IRQn);
  Nucleo_SetPinGPIO(RGB_BLUE, ioPortOutputPP);

  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  /* TODO - Add your application code here */

  /* Infinite loop */
  while (1)
  {
    if (tim3irq) // priznak ze doslo k preteceni
    {
    tim3irq = false; // shodit
    GPIOToggle(RGB_BLUE);
    }
  }
}
