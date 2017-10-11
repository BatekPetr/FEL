/*
 * mina_shield.h
 *
 *  Created on: Sep 23, 2017
 *      Author: petr
 */

#ifndef MINA_SHIELD_H_
#define MINA_SHIELD_H_

#include "nucleo_core.h"

#define RGB_BLUE    GPIOA,9
#define RGB_GREEN   GPIOC,7
#define RGB_RED     GPIOB,4

#define JOY_UP      GPIOA,4
#define JOY_DOWN    GPIOB,0
#define JOY_LEFT    GPIOC,1
#define JOY_RIGHT   GPIOC,0
#define JOY_PUSH    GPIOB,5

#define UP_PRESSED    0b10000
#define DOWN_PRESSED  0b01000
#define LEFT_PRESSED  0b00100
#define RIGHT_PRESSED 0b00010
#define PUSH_PRESSED  0b00001

void initJoystick();
uint8_t readJoystick();

#endif /* MINA_SHIELD_H_ */
