/*
 * mina_shield.c
 *
 *  Created on: Oct 4, 2017
 *      Author: batekp
 */


#include "mina_shield.h"

uint8_t readJoystick()
{
  uint8_t up = 0;
  uint8_t down = 0;
  uint8_t left = 0;
  uint8_t right = 0;
  uint8_t pushed = 0;

  if (GPIORead(JOY_UP))
    up = 0b10000;
  if (GPIORead(JOY_DOWN))
    down = 0b01000;
  if (GPIORead(JOY_LEFT))
    left = 0b00100;
  if (GPIORead(JOY_RIGHT))
    right = 0b00010;
  if (GPIORead(JOY_PUSH))
    pushed = 0b00001;

  return up + down + left + right + pushed;
}
