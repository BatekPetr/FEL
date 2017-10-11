/*
 * mandelbrot.c
 *
 *  Created on: Oct 8, 2017
 *      Author: petr
 */
#include "mina_shield.h"
#include "mbed_shield_lcd.h"        // include public functions

volatile uint32_t _ticks = 0;

const int pixelWidth = 128, pixelHeight = 32;
const float centerXreset = 1.5f, centerYreset = 1.0f,
  zoomXreset = 1, zoomYreset = 1;

float centerX = 0, centerY = 0;
float zoomX = 1, zoomY = 1;

void CalculateMandel()
{
  GPIOToggle(RGB_RED);
  uint32_t start_tick = _ticks;

  for (int yy = 0; yy < pixelHeight; yy++)
  {
    for (int xx = 0; xx < pixelWidth; xx++)
    {
      float x0 = xx;
      float y0 = yy;
      x0 = x0 / (pixelWidth / 2 * zoomX) - centerX;
      y0 = y0 / (pixelHeight / 2 * zoomY) - centerY;
      // z = z * z + c

      float x = 0;
      float y = 0;
      float xtemp = 0;

      int iter = 0;

      while (((x * x + y * y) < 4) && (iter < 9))
      {
        xtemp = x * x - y * y + x0;
        y = 2 * x * y + y0;
        x = xtemp;
        iter++;
      }

      float z = x * x + y * y;

      // Uprava pro MINA shield
      if(z < 4)
        MBED_LCD_PutPixel(xx, yy, true);
      else
        MBED_LCD_PutPixel(xx, yy, false);

    }
  }
  GPIOToggle(RGB_RED);
  uint32_t ms_elapsed = _ticks - start_tick;
  char buf[128];
  sprintf(buf, "Time: %u", (int)ms_elapsed);
  MBED_LCD_WriteStringXY(buf, 0, 0);    // example string output

  MBED_LCD_VideoRam2LCD();          // move changes in video buffer to LCD
}

