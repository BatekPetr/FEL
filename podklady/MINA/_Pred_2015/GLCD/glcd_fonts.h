#ifndef __GLCD_FONTS_H
#define __GLCD_FONTS_H

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct sGLCDFont_struct
{
  void *baseAdr;
  uint8_t width;
  uint8_t height;
  uint8_t offsetChar;
  int msbFirst;
  const char *name;
} sGLCDFont;

extern const sGLCDFont glcdFonts[];

#endif
