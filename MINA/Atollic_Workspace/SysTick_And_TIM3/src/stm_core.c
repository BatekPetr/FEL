/*
 * stm_core.c
 *
 *  Created on: Sep 23, 2017
 *      Author: petr
 */


#include "stm_core.h"

bool Nucleo_SetPinGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, eIoPortModes mode)
{
  uint32_t enr_mask = 0;        // hodnota do xxENR registru
  uint32_t rstr_mask = 0;       // hodnota do xxRSTR registru

  switch((uint32_t)gpio)    // detekce, ktery GPIO
  {
    case (uint32_t)GPIOA:
      enr_mask = RCC_AHB1ENR_GPIOAEN;
      rstr_mask = RCC_AHB1RSTR_GPIOARST;
      break;
    case (uint32_t)GPIOB:
      enr_mask = RCC_AHB1ENR_GPIOBEN;
      rstr_mask = RCC_AHB1RSTR_GPIOBRST;
      break;
    case (uint32_t)GPIOC:
        enr_mask = RCC_AHB1ENR_GPIOCEN;
        rstr_mask = RCC_AHB1RSTR_GPIOCRST;
        break;
    case (uint32_t)GPIOD:
        enr_mask = RCC_AHB1ENR_GPIODEN;
        rstr_mask = RCC_AHB1RSTR_GPIODRST;
        break;
    case (uint32_t)GPIOE:
        enr_mask = RCC_AHB1ENR_GPIOEEN;
        rstr_mask = RCC_AHB1RSTR_GPIOERST;
        break;
#if defined(STM32F411xE) // 411RE nema GPIOF a G
#else
    case (uint32_t)GPIOF:
        enr_mask = RCC_AHB1ENR_GPIOFEN;
        rstr_mask = RCC_AHB1RSTR_GPIOFRST;
        break;
    case (uint32_t)GPIOG:
        enr_mask = RCC_AHB1ENR_GPIOGEN;
        rstr_mask = RCC_AHB1RSTR_GPIOGRST;
        break;
#endif
    case (uint32_t)GPIOH:
        enr_mask = RCC_AHB1ENR_GPIOHEN;
        rstr_mask = RCC_AHB1RSTR_GPIOHRST;
        break;
  }

  if ((enr_mask == 0) || (rstr_mask == 0))    // nevybran GPIO
      return false;                           // vrat priznak chyby


  if (!(RCC->AHB1ENR & enr_mask))             // inicializace vybraneho
  {
    RCC->AHB1ENR |= enr_mask;                 // povolit hodiny periferie
    RCC->AHB1RSTR |= rstr_mask;               // proved reset periferie
    RCC->AHB1RSTR &= ~rstr_mask;              // a konec resetu
  }

  // nastaveni konfiguracnich bitu do defaultniho stavu (nemususelo byt od drive)
  gpio->MODER &= ~(0x03 << (2 * bitnum));   // vynuluj prislusne 2 bity v registru
  gpio->PUPDR &= ~(0x03 << (2 * bitnum));   // vynuluj prislusne 2 bity v registru
  gpio->OSPEEDR &= ~(0x03 << (2 * bitnum)); // vynuluj prislusne 2 bity v registru

  // nastaveni registru podle typu vystupu/vstupu
  switch(mode)
  {
    case ioPortOutputOC:
    case ioPortOutputPP:
      gpio->MODER |= 0x01 << (2 * bitnum);    // 01 = output
      gpio->OSPEEDR |= 0x03 << (2 * bitnum);  // 11 = high speed
      gpio->PUPDR &=  ~(0x03 << (2 * bitnum));  // 00 = no pu/pd

      if (mode == ioPortOutputOC)   // open collector (drain) ?
        gpio->OTYPER |= 0x01 << bitnum;   // 1 = OC/Open drain
      else
        gpio->OTYPER &= ~(0x01 << bitnum);// 0 = push-pull
      break;
     case ioPortInputPU:                  // moder bits 00 = input
       gpio->PUPDR &= ~(0x03 << (2 * bitnum));  // clear bits
       gpio->PUPDR |= 0x01 << (2 * bitnum);     // 01 = pull-up
       break;
     case ioPortInputPD:
       gpio->PUPDR &= ~(0x03 << (2 * bitnum));  // clear bits
       gpio->PUPDR |= 0x02 << (2 * bitnum);     // 10 = pull-up
       break;
     case ioPortInputFloat:     // 00 = input mode, nothing to do
        gpio->PUPDR &= ~(0x03 << (2 * bitnum)); // 00 = no pull-up/dn
        break;
     case ioPortAnalog:         // 11 - analog mode
       gpio->MODER |= 0x03 << (2 * bitnum);   // set bits
       break;
     case ioPortAlternatePP:
     case ioPortAlternateOC:
       gpio->MODER |= 0x02 << (2 * bitnum); // set bits

       if (mode == ioPortAlternateOC)
         gpio->OTYPER |= 0x01 << bitnum; // 1 = open-drain
       else
         gpio->OTYPER &= ~(0x01 << bitnum); // 0 = push-pull
	
        gpio->OSPEEDR |= 0x03 << (2 * bitnum); // high-speed = 11
        gpio->PUPDR &= ~(0x03 << (2 * bitnum)); // 00 = no pull-up/pull-down
                    // don't forget set AFR registers !!!
        break;
     default:   // neznamy rezim ?
       return false; // priznak chyby
  }

  return true;  // priznak OK
}

void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitnum)
{
  gpio->ODR ^= (1 << bitnum);
}

bool GPIORead(GPIO_TypeDef *gpio, uint32_t bitnum)
{
  return ((gpio->IDR & (1 << bitnum)) != 0);    // porovnani = vynucena logicka hodnota
}

void GPIOWrite(GPIO_TypeDef *gpio, uint32_t bitnum, bool state)
{
  gpio->BSRR = (state) ? (1 << (bitnum)) : ((1 << (bitnum)) << 16);
              // BSRR register - lower 16 bits = Set to 1
              // higher 16 bits = Reset to 0
}





