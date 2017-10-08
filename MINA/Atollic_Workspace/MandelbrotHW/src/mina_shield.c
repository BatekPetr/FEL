/*
 * mina_shield.c
 *
 *  Created on: Oct 4, 2017
 *      Author: batekp
 */


#include "mina_shield.h"

void initJoystick()
{
  Nucleo_SetPinGPIO(JOY_UP, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_DOWN, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_LEFT, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_RIGHT, ioPortInputFloat);
  Nucleo_SetPinGPIO(JOY_PUSH, ioPortInputFloat);
}
uint8_t readJoystick()
{
  uint8_t up = 0;
  uint8_t down = 0;
  uint8_t left = 0;
  uint8_t right = 0;
  uint8_t pushed = 0;

  if (GPIORead(JOY_UP))
    up = UP_PRESSED;
  if (GPIORead(JOY_DOWN))
    down = DOWN_PRESSED;
  if (GPIORead(JOY_LEFT))
    left = LEFT_PRESSED;
  if (GPIORead(JOY_RIGHT))
    right = RIGHT_PRESSED;
  if (GPIORead(JOY_PUSH))
    pushed = PUSH_PRESSED;

  return up + down + left + right + pushed;
}
