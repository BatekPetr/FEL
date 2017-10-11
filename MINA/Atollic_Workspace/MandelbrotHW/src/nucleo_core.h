/*
 * stm_core.h
 *
 *  Created on: Sep 23, 2017
 *      Author: petr
 */

#ifndef NUCLEO_CORE_H_
#define NUCLEO_CORE_H_

#include"stm32f4xx.h"
#include <stdbool.h>

typedef enum {
  ioPortOutputPP,     // vystup typu Push-Pull
  ioPortOutputOC,     // vystup typu Open Collector
  ioPortAnalog,       // analogovy vstup - pro prevodnik
  ioPortInputFloat,   // vstup bez pull-up/down
  ioPortInputPU,      // vstup s pull-up
  ioPortInputPD,      // vstup s pull-down
  ioPortAlternatePP,  // alternativni vystup - push/pull
  ioPortAlternateOC   // alternativni vystup - open drain
} eIoPortModes;

typedef enum {
  busClockAHB,
  busClockAPB1,
  busClockAPB2,
  timersClockAPB1,
  timersClockAPB2
} eBusClocks;

typedef enum {
  clockSourceHSI,
  clockSourceHSE
} eClockSources;

bool Nucleo_SetPinAFGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, uint32_t afValue);
bool Nucleo_SetPinGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, eIoPortModes mode);
void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitnum);
bool GPIORead(GPIO_TypeDef *gpio, uint32_t bitnum);
void GPIOWrite(GPIO_TypeDef *gpio, uint32_t bitnum, bool state);

int Usart2Send(char c);
int Usart2Recv(void);
bool IsUsart2Recv(void);
void Usart2String(char *txt);
void Usart2String(char *txt);
int Usart2SendStr(char c[]);

uint32_t GetBusClock(eBusClocks clk);
bool SetClock100MHz(eClockSources clkSrc);
bool SetClockHSI(void);

#define BOARD_BTN_BLUE GPIOC,13
#define BOARD_LED GPIOA,5

#endif /* NUCLEO_CORE_H_ */
