/*
 * stm_core.c
 *
 *  Created on: Sep 23, 2017
 *      Author: petr
 */


#include <nucleo_core.h>

// GPIO functions

bool Nucleo_SetPinAFGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, uint32_t afValue)
{
  gpio->AFR[(bitnum < 8) ? 0 : 1] &= ~(0x0f << (4 * (bitnum & 0x07))); // vynuluj AF bity
  gpio->AFR[(bitnum < 8) ? 0 : 1] |= ((afValue & 0x0f) << (4 * (bitnum & 0x07))); // nastav AF bity
  return true;
}

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

uint32_t GetTimerClock(int timerNum)
{
  uint32_t timerClock = 0;

#if defined(STM32F411xE)  // | defined ...
  switch(timerNum)
  {
    case 1:               // timers on APB2
    case 9:
    case 10:
    case 11:
      timerClock = GetBusClock(timersClockAPB2);
      break;
    case 2:              // timers on APB1
    case 3:
    case 4:
    case 5:
      timerClock = GetBusClock(timersClockAPB1);
      break;
  }
#endif

  return timerClock;
}

uint32_t GetBusClock(eBusClocks clk)
{
  uint32_t bitval = 0;
  uint32_t divider = 1;

#if defined(STM32F411xE)  // | defined ...
  switch(clk)
  {
    case busClockAHB:
      bitval = (RCC->CFGR & (0x0f << 4)) >> 4;   // HPRE [7:4] to lower 4 bits
      if (bitval & 0x8)           // 1xxx
        divider = 1 << ((bitval & 0x07) + 1);   // 0 = /2, 1 = /4
      else
        divider = 1;              // 0xxx = not divided
      break;
    case busClockAPB1:
    case timersClockAPB1:         // x2
      bitval = (RCC->CFGR & (0x07 << 10)) >> 10; // PPRE1 [12:10] to lower 3 bits
      if (bitval & 0x4)           // 1xx
        divider = 1 << ((bitval & 0x03) + 1);   // 0 = /2, 1 = /4
      else
        divider = 1;              // 0xx = not divided

      break;
    case busClockAPB2:
    case timersClockAPB2:         // the same
      bitval = (RCC->CFGR >> 13) & 0x07; // PPRE2 [15:13] to lower 3 bits
      if (bitval & 0x4)           // 1xx
        divider = 1 << ((bitval & 0x03) + 1);   // 0 = /2, 1 = /4
      else
        divider = 1;              // 0xx = not divided
      break;
    default:
      return 0;
  }

  SystemCoreClockUpdate();      // pro jistotu si nastav SystemCoreClock

  if (((clk == timersClockAPB1) || (clk == timersClockAPB1)) && (divider > 1))
    return SystemCoreClock / divider * 2;
  else
    return SystemCoreClock / divider;
#else
#error Valid controller not set
#endif
}

bool SetClock100MHz(eClockSources clkSrc)
{
  uint32_t t;

#if defined(STM32F411xE)  // || dalsi modely se stejnym CR/CFGR
#else
#error Unsupported processor
#endif

#if HSE_VALUE != 8000000
#error HSE_VALUE must be set to 8M = ext. clock from ST/Link on Nucleo
#endif

  if (clkSrc == clockSourceHSE)
  {
    if (!(RCC->CR & RCC_CR_HSEON))      // HSE not running ?
    {
      RCC->CR |= RCC_CR_HSEON;          // enable

      t = 200;
      while(!(RCC->CR & RCC_CR_HSEON) && t)   // wait to ON
        t--;
      if (!t)
        return false;
    }
  }

  if (!(RCC->CR & RCC_CR_HSION))      // HSI not running ?
  {
    RCC->CR |= RCC_CR_HSION;          // enable

    t = 100;
    while(!(RCC->CR & RCC_CR_HSION) && t)   // wait to ON
      t--;
    if (!t)
      return false;
  }

  if (RCC->CR & RCC_CR_PLLON)         // bezi ?
  {
    RCC->CR &= ~RCC_CR_PLLON;         // stop it
  }

  RCC->CFGR &= ~(RCC_CFGR_SW);  // SW = 00 - HSI as source

  RCC->CFGR = 0;       // RESET state, all off

  RCC->CFGR |= 0
      | 0 << 13        // PPRE2 [15:13] = 0xx = not divided
      | 4 << 10        // PPRE1 [12:10] = 100 = /2 (max. 50MHz)
      | 0 << 4         // HPRE  [7:4] = 0xxx = not divided
      ;

  if (clkSrc == clockSourceHSI)
  {
    RCC->PLLCFGR &= ~(1 << 22);   // PLLSRC [22] = 0 = HSI as source

    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
    RCC->PLLCFGR |= 8 << 0;       // PLLM [5:0] = odpovida deleni
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
    RCC->PLLCFGR |= 100 << 6;     // PLLN [14:6] = odpovida deleni
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
    RCC->PLLCFGR |= 0 << 16;      // PLLP [17:16] = 00 = /2
  }

  if (clkSrc == clockSourceHSE)
  {
    RCC->PLLCFGR &= ~(1 << 22);   // PLLSRC [22] = 0 = HSI as source
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;

    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
    RCC->PLLCFGR |= 4 << 0;       // PLLM [5:0] = odpovida deleni
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
    RCC->PLLCFGR |= 100 << 6;     // PLLN [14:6] = odpovida deleni
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;
    RCC->PLLCFGR |= 0 << 16;      // PLLP [17:16] = 00 = /2
  }

  RCC->CR |= RCC_CR_PLLON;          // enable

  t = 100;
  while(!(RCC->CR & RCC_CR_PLLON) && t)   // wait to ON
    t--;
  if (!t)
    return false;

  // RM - 3.4 Read interface
  FLASH->ACR &= ~(0x0f << 0);     // LATENCY [3:0] = 0000
  FLASH->ACR |= (3 << 0);         // 3 WS

  PWR->CR |= PWR_CR_VOS_0 | PWR_CR_VOS_1;   // scale mode 1 - req. for 100MHz

  RCC->CFGR |= RCC_CFGR_SW_PLL;
  t = 100;
  while(!((RCC->CFGR & 0x0c) == RCC_CFGR_SWS_PLL) && t)   // wait to verify SWS
    t--;
  if (!t)
    return false;

  return true;
}

bool SetClockHSI(void)
{
  uint32_t t;

  if (!(RCC->CR & RCC_CR_HSION))      // HSI not running ?
  {
    RCC->CR |= RCC_CR_HSION;          // enable

    t = 100;
    while(!(RCC->CR & RCC_CR_HSION) && t)   // wait to ON
      t--;
    if (!t)
      return false;
  }

  RCC->CFGR &= ~RCC_CFGR_SW;         // clear SW bits
  t = 100;
  while(!((RCC->CFGR & 0x0c) == RCC_CFGR_SWS_HSI) && t)   // wait to verify SWS
    t--;
  if (!t)
    return false;

  return true;
}

/*
bool SetClock100MHz(eClockSources clkSrc)
{
  // Prepinac zdroju hodin nastavit na HSE
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_HSE;

  // Povolit volbu BYPASS
  RCC->CR |= RCC_CR_HSEBYP;

  // Nastavit Input Frekvenci HSE na 8 MHz

  // Nastav PLL source na HSE
  RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
}

bool SetClockHSI(void)
{
  // Prepinac zdroju hodin nastavit na HSI
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_HSI;

  // Zapnout HSI
  RCC->CR |= RCC_CR_HSION;
}
*/


