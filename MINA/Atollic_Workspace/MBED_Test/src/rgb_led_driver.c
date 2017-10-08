/*
 * rgb_led_driver.c
 *
 *  Created on: Oct 3, 2017
 *      Author: petr
 */

#include "rgb_led_driver.h"

void initRGB_LED_PWM()
{
  uint32_t apb2 = SystemCoreClock; //TODO dopln podle RCC

  // ----------------------------------------------------
  // BLUE LED
  if (!(RCC->APB2ENR & RCC_APB2ENR_TIM1EN))
  {
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
  }
  TIM1->CR1 = 0 // DIR = 0 - upcounter
      | TIM_CR1_ARPE; // bufferovany zapis do ARR
  TIM1->CR2 = 0;
  TIM1->PSC = apb2 / 1E6 - 1; // 1MHz - 1us
  TIM1->ARR = 256 - 1; // 256us preteceni
  TIM1->CR1 = TIM_CR1_CEN;
  TIM1->BDTR |= TIM_BDTR_MOE;
  Nucleo_SetPinGPIO(RGB_BLUE, ioPortAlternatePP); // BLUE
  Nucleo_SetPinAFGPIO(RGB_BLUE, 1); // AF01 odpovida TIM1_CH2

  TIM1->CCMR1 &= ~TIM_CCMR1_OC2M;
  TIM1->CCMR1 |= TIM_CCMR1_OC2M; //TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; // 110 – PWM 1

  TIM1->CCER |= TIM_CCER_CC2E; // povol vystup
  TIM1->CCR2 = 0; //TIM1->ARR / 4;
  // ------------------------------------------------------

  // RED and GREEN LED
  uint32_t apb1 = SystemCoreClock; //TODO dopln podle RCC

  if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN))
  {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
  }
  TIM3->CR1 = 0 // DIR = 0 - upcounter
      | TIM_CR1_ARPE; // bufferovany zapis do ARR
  TIM3->CR2 = 0;
  TIM3->PSC = apb1 / 1E6 - 1; // 1MHz - 1us
  TIM3->ARR = 256 - 1; // 1kHz - 1ms preteceni
  TIM3->CR1 = TIM_CR1_CEN;
  TIM3->BDTR |= TIM_BDTR_MOE;

  // RED LED
  Nucleo_SetPinGPIO(RGB_RED, ioPortAlternatePP);
  Nucleo_SetPinAFGPIO(RGB_RED, 2); // AF02 odpovida TIM3_CH1
  TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
  TIM3->CCMR1 |= TIM_CCMR1_OC1M; //TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // 110 - PWM 1
  TIM3->CCER |= TIM_CCER_CC1E; // povol vystup
  TIM3->CCR1 = 0; //TIM1->ARR / 2;

  // GREEN LED
  Nucleo_SetPinGPIO(RGB_GREEN, ioPortAlternatePP); // GREEN
  Nucleo_SetPinAFGPIO(RGB_GREEN, 2); // AF02 odpovida TIM3_CH2
  TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;
  TIM3->CCMR1 |= TIM_CCMR1_OC2M; //TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; // 110 - PWM 1
  TIM3->CCER |= TIM_CCER_CC2E; // povol vystup
  TIM3->CCR2 = 0; //TIM1->ARR / 2;
  // -------------------------------------------------------
}

void set_RGB_LED_PWM(uint32_t red_PWM, uint32_t green_PWM, uint32_t blue_PWM)
{
  if (red_PWM > 255)
    red_PWM = 255;
  if (green_PWM > 255)
      green_PWM = 255;
  if (blue_PWM > 255)
      blue_PWM = 255;

  TIM3->CCR1 = red_PWM;
  TIM3->CCR2 = green_PWM;
  TIM1->CCR2 = blue_PWM;
}
