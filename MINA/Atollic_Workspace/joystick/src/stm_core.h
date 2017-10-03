/*
 * stm_core.h
 *
 *  Created on: Sep 23, 2017
 *      Author: petr
 */

#ifndef STM_CORE_H_
#define STM_CORE_H_

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

bool Nucleo_SetPinGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, eIoPortModes mode);
void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitnum);
bool GPIORead(GPIO_TypeDef *gpio, uint32_t bitnum);
void GPIOWrite(GPIO_TypeDef *gpio, uint32_t bitnum, bool state);

#define BOARD_BTN_BLUE GPIOC,13
#define BOARD_LED GPIOA,5

#endif /* STM_CORE_H_ */
