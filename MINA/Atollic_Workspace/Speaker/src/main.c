/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 8.0.0   2017-10-02

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
  uint32_t apb1 = SystemCoreClock; //TODO dopln podle RCC

  if (!(RCC->APB1ENR & RCC_APB1ENR_TIM2EN))
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
  }
  TIM2->CR1 = 0 // DIR = 0 - upcounter
      | TIM_CR1_ARPE; // bufferovany zapis do ARR
  TIM2->CR2 = 0;
  TIM2->PSC = apb1 / 1E6 - 1; // 1MHz - 1us
  TIM2->ARR = 10000 - 1; // 1kHz - 1ms preteceni
  TIM2->CR1 = TIM_CR1_CEN;

  Nucleo_SetPinGPIO(GPIOB, 10, ioPortAlternatePP);
  Nucleo_SetAFGPIO(GPIOB, 10, 1); // AF01 = TIM2_CH3

  TIM2->CCMR2 &= ~TIM_CCMR2_OC3M;
  TIM2->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;

  // 110 - PWM mode 1
  TIM2->CCER |= TIM_CCER_CC3E;
  TIM2->CCR3 = TIM2->ARR / 2;
}
