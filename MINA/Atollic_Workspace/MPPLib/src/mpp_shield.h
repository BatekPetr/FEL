/*
 * mpp_shield.h
 *
 *  Created on: Sep 23, 2017
 *      Author: petr
 */

#ifndef MPP_SHIELD_H_
#define MPP_SHIELD_H_

#include "stm_core.h"

void Init8LED(void);
void Write8LED(uint8_t val);

#define RGB_BLUE    GPIOB,13
#define RGB_GREEN   GPIOB,14
#define RGB_RED     GPIOB,15

#define BTN_LEFT    GPIOB, 5
#define BTN_RIGHT   GPIOB, 4

#endif /* MPP_SHIELD_H_ */
