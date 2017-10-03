/*
 * mpp_shield.c
 *
 *  Created on: Sep 23, 2017
 *      Author: petr
 */

#include "mpp_shield.h"

void Init8LED(void)
{
  if (!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOCEN))  // neni aktivni GPIOC ?
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;      // povol hodiny
    RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOCRST;   // udelej RESET puls periferii
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOCRST;
  }



  GPIOC->MODER &= 0xffff0000;   // nizsich 8 I/O nastaveno na 00 kazdy = defaultni stav
  GPIOC->MODER |= 0x00005555;   // a pak jim nastav kazdemu 01 = output, bitove zapis ... 0101 0101 0101


  GPIOC->OTYPER &= 0xff00;    // kazdy vystup reprezentuje 1 bit, 0 = push-pull

  GPIOC->OSPEEDR |= 0x0000ffff;   // 2 bity na vystup, kombinace 11 = high speed
  return;
}

void Write8LED(uint8_t val)
{
  GPIOC->ODR = (GPIOC->ODR & 0xff00) | ((uint16_t)val);   // zachovat hornich 8b a prepsat spodnich 8b hodnotou
  return;
}
