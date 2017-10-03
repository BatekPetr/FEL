/******************************************************************************/
/* GLCD_SPI_STM32.c: STM32 low level Graphic LCD (240x320 pixels) driven      */
/*                   with SPI functions                                       */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2011 Keil - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#pragma diag_suppress=550

#include <stm32f10x.h>
#include <string.h>
#include "glcd_mcbstm32c.h"
#include "glcd_fonts.h"

/************************** Orientation  configuration ************************/

static int _orientLandscape = 0;       /* 1 for landscape, 0 for portrait    */
static int _orientRotate180 = 0;       /* 1 to rotate the screen for 180 deg */

//#define ENABLE_TEXT_TRANSPARENCY

/*********************** Hardware specific configuration **********************/
/* SPI Interface: SPI3
   
   PINS: 
   - CS     = PB10 (GPIO pin)
   - RS     = GND
   - WR/SCK = PC10 (SPI3_SCK)
   - RD     = GND
   - SDO    = PC11 (SPI3_MISO)
   - SDI    = PC12 (SPI3_MOSI)                                                */

#define PIN_CS      (1 << 10)
#define PIN_CLK     (1 << 10)
#define PIN_DAT     (1 << 12)

#define IN          0x00
#define OUT         0x01

/* SPI_SR - bit definitions                                                   */
#define RXNE        0x01
#define TXE         0x02
#define BSY         0x80

/*------------------------- Speed dependant settings -------------------------*/
/* If processor works on high frequency delay has to be increased, it can be 
   increased by factor 2^N by this constant                                   */
#define DELAY_2N    18

/*---------------------- Graphic LCD size definitions ------------------------*/

static int _width = 0;
static int _height = 0;

#define BPP         16                  /* Bits per pixel                     */
#define BYPP        ((BPP+7)/8)         /* Bytes per pixel                    */

/*--------------- Graphic LCD interface hardware definitions -----------------*/

/* Pin CS setting to 0 or 1                                                   */
#define LCD_CS(x)   ((x) ? (GPIOB->BSRR = PIN_CS)  : (GPIOB->BRR = PIN_CS))
#define LCD_CLK(x)  ((x) ? (GPIOC->BSRR = PIN_CLK) : (GPIOC->BRR = PIN_CLK))
#define LCD_DAT(x)  ((x) ? (GPIOC->BSRR = PIN_DAT) : (GPIOC->BRR = PIN_DAT))
#define DAT_MODE(x) ((x == OUT) ? (GPIOC->CRH = GPIOC->CRH & ~(0xF<<16)|(3<<16)) : \
                                  (GPIOC->CRH = GPIOC->CRH & ~(0xF<<16)|(1<<18)))
#define BUS_VAL()                ((GPIOC->IDR & PIN_DAT) != 0)

#define SPI_START   (0x70)              /* Start byte for SPI transfer        */
#define SPI_RD      (0x01)              /* WR bit 1 within start              */
#define SPI_WR      (0x00)              /* WR bit 0 within start              */
#define SPI_DATA    (0x02)              /* RS bit 1 within start byte         */
#define SPI_INDEX   (0x00)              /* RS bit 0 within start byte         */

#define BG_COLOR  0                     /* Background color                   */
#define TXT_COLOR 1                     /* Text color                         */

 
/*---------------------------- Global variables ------------------------------*/

/******************************************************************************/
static volatile unsigned short Color[2] = {White, Black};

/************************ Local auxiliary functions ***************************/

/*******************************************************************************
* Delay in while loop cycles                                                   *
*   Parameter:    cnt:    number of while cycles to delay                      *
*   Return:                                                                    *
*******************************************************************************/
static void delay (int cnt) 
{
  cnt <<= DELAY_2N;
  while (cnt--);
}


/*******************************************************************************
* Transfer 1 byte over the serial communication                                *
*   Parameter:    byte:   byte to be sent                                      *
*   Return:               byte read while sending                              *
*******************************************************************************/
static __inline unsigned char spi_tran (unsigned char byte) 
{
  SPI3->DR = byte;
  while (!(SPI3->SR & RXNE));           /* Wait for send to finish            */
  return (SPI3->DR);
}


/*******************************************************************************
* Write a command the LCD controller                                           *
*   Parameter:    cmd:    command to be written                                *
*   Return:                                                                    *
*******************************************************************************/
static __inline void wr_cmd (unsigned char cmd) 
{
  LCD_CS(0);
  spi_tran(SPI_START | SPI_WR | SPI_INDEX);   /* Write : RS = 0, RW = 0       */
  spi_tran(0);
  spi_tran(cmd);
  LCD_CS(1);
}


/*******************************************************************************
* Write data to the LCD controller                                             *
*   Parameter:    dat:    data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/
static __inline void wr_dat (unsigned short dat) 
{
  LCD_CS(0);
  spi_tran(SPI_START | SPI_WR | SPI_DATA);    /* Write : RS = 1, RW = 0       */
  spi_tran((dat >>   8));                     /* Write D8..D15                */
  spi_tran((dat & 0xFF));                     /* Write D0..D7                 */
  LCD_CS(1);
}


/*******************************************************************************
* Start of data writing to the LCD controller                                  *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
static __inline void wr_dat_start (void) 
{
  LCD_CS(0);
  spi_tran(SPI_START | SPI_WR | SPI_DATA);    /* Write : RS = 1, RW = 0       */
}


/*******************************************************************************
* Stop of data writing to the LCD controller                                   *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
static __inline void wr_dat_stop (void) 
{
  LCD_CS(1);
}


/*******************************************************************************
* Data writing to the LCD controller                                           *
*   Parameter:    dat:    data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/
static __inline void wr_dat_only (unsigned short dat) 
{
  spi_tran((dat >>   8));                     /* Write D8..D15                */
  spi_tran((dat & 0xFF));                     /* Write D0..D7                 */
}


/*******************************************************************************
* Write a value to the to LCD register                                         *
*   Parameter:    reg:    register to be written                               *
*                 val:    value to write to the register                       *
*******************************************************************************/
static __inline void wr_reg (unsigned char reg, unsigned short val) 
{
  wr_cmd(reg);
  wr_dat(val);
}


#if 0
/*******************************************************************************
* Transfer 1 byte over the serial communication                                *
*   Parameter:    byte:   byte to be sent                                      *
*                 mode:   OUT = transmit byte, IN = receive byte               *
*   Return:               byte read while sending                              *
*******************************************************************************/
static unsigned char spi_tran_man (unsigned char byte, unsigned int mode) 
{
  unsigned char val = 0;
  int i;

  if (mode == OUT) { DAT_MODE (OUT); }
  else             { DAT_MODE (IN);  }

  for (i = 7; i >= 0; i--) 
  {
    LCD_CLK(0);
    delay(1);
    if (mode == OUT) 
    {
      LCD_DAT((byte & (1 << i)) != 0);
    }
    else 
    {
      val |= (BUS_VAL() << i);
    }
    LCD_CLK(1);
    delay(1);
  }
  return (val);
}


/*******************************************************************************
* Read data from the LCD controller                                            *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/
static __inline unsigned short rd_dat (void) 
{
  unsigned short val = 0;

  LCD_CS(0);
  spi_tran(SPI_START | SPI_RD | SPI_DATA);    /* Read: RS = 1, RW = 1         */
  spi_tran(0);                                /* Dummy read 1                 */  
  val   = spi_tran(0);                        /* Read D8..D15                 */
  val <<= 8;
  val  |= spi_tran(0);                        /* Read D0..D7                  */
  LCD_CS(1);
  return (val);
}


/*******************************************************************************
* Write a command the LCD controller                                           *
*   Parameter:    cmd:    command to be written                                *
*   Return:                                                                    *
*******************************************************************************/
static __inline void rd_cmd (unsigned char cmd) 
{
  LCD_CS(0);
  spi_tran(SPI_START | SPI_RD | SPI_INDEX);   /* Write : RS = 0, RW = 1       */
  spi_tran(0);
  spi_tran(cmd);
  LCD_CS(1);
}


/*******************************************************************************
* Start of data writing to the LCD controller                                  *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
static __inline void rd_dat_start (void) 
{
  LCD_CS(0);
  spi_tran(SPI_START | SPI_RD | SPI_DATA);    /* Write : RS = 1, RW = 0       */
  spi_tran(0);                                /* Dummy read 1                 */  
}


/*******************************************************************************
* Stop of data writing to the LCD controller                                   *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/
static __inline void rd_dat_stop (void) 
{
  LCD_CS(1);
}


/*******************************************************************************
* Data writing to the LCD controller                                           *
*   Parameter:    dat:    data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/
static __inline unsigned short rd_dat_only (void) 
{
  unsigned short val = 0;

  val   = spi_tran(0);                        /* Read D8..D15                 */
  val <<= 8;
  val  |= spi_tran(0);                        /* Read D0..D7                  */
  
  return val;
}


/*******************************************************************************
* Read from the LCD register                                                   *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from the register                         *
*******************************************************************************/
static unsigned short rd_reg (unsigned char reg)
{
  wr_cmd(reg);
  return(rd_dat());
}


/*******************************************************************************
* Read LCD controller ID (Himax GLCD)                                          *
*   Parameter:    (none)                                                       *
*   Return:       controller ID                                                *
*******************************************************************************/
static unsigned short rd_id_man (void) 
{
  unsigned short val;

  /* Set MOSI, MISO and SCK as GPIO pins, with pull-down/pull-up disabled     */
  GPIOC->CRH   &= 0xFFF000FF;
  GPIOC->CRH   |= 0x00033300;

  LCD_CS (1);                           /* Set chip select high               */
  LCD_CLK(1);                           /* Set clock high                     */

  LCD_CS(0);
  spi_tran_man (SPI_START | SPI_WR | SPI_INDEX, OUT);
  spi_tran_man (0x00, OUT);
  LCD_CS(1);

  LCD_CS(0);
  spi_tran_man (SPI_START | SPI_RD | SPI_DATA, OUT);
  val = spi_tran_man(0, IN);
  LCD_CS(1);

  /* Connect MOSI, MISO, and SCK to SSP peripheral                            */
  GPIOC->CRH   &= 0xFFF000FF;
  GPIOC->CRH   |= 0x000B8B00;
  return (val);
}


#endif

/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Init (int landscape, int rotate) 
{
  if (landscape)
  {
    _orientLandscape = 1;
    _width = 320;
    _height = 240;
  }
  else
  {
    _orientLandscape = 0;
    _width = 240;
    _height = 320;
  }
  
  _orientRotate180 = rotate ? 1 : 0;

  /* Enable clock for GPIOB,C AFIO and SPI3                                   */
  RCC->APB2ENR |= 0x00000019;
  RCC->APB1ENR |= 0x00008000;

  /* Set SPI3 remap (use PC10..PC12)                                          */
  AFIO->MAPR   |= 0x10000000;

  /* NCS is PB10, GPIO output set to high                                     */
  GPIOB->CRH   &= 0xFFFFF0FF;
  GPIOB->CRH   |= 0x00000300;
  GPIOB->CRL   &= 0xFFFFFFF0;
  GPIOB->CRL   |= 0x00000003;
  GPIOB->BSRR   = 0x00000401;

  /* SPI3_SCK, SPI3_MISO, SPI3_MOSI are SPI pins                              */
  GPIOC->CRH   &= 0xFFF000FF;
  GPIOC->CRH   |= 0x000B8B00;

  /* Enable SPI in Master Mode, CPOL=1, CPHA=1                                */
  /* Max. 18 MBit used for Data Transfer @ 72MHz                              */
  //SPI3->CR1     = 0x0347;
  SPI3->CR1 = SPI_CR1_SSM | SPI_CR1_SSI |
    SPI_CR1_SPE |
    SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_MSTR |
    0;   // 000 = fclk / 2
  SPI3->CR2     = 0x0000;

  {
    /* Start Initial Sequence ------------------------------------------------*/
    if (_orientRotate180)
      wr_reg(0x01, 0x0000);               /* Clear SS bit                       */
    else
      wr_reg(0x01, 0x0100);               /* Set SS bit                         */

    wr_reg(0x02, 0x0700);               /* Set 1 line inversion               */
    wr_reg(0x04, 0x0000);               /* Resize register                    */
    wr_reg(0x08, 0x0207);               /* 2 lines front, 7 back porch        */
    wr_reg(0x09, 0x0000);               /* Set non-disp area refresh cyc ISC  */
    wr_reg(0x0A, 0x0000);               /* FMARK function                     */
    wr_reg(0x0C, 0x0000);               /* RGB interface setting              */
    wr_reg(0x0D, 0x0000);               /* Frame marker Position              */
    wr_reg(0x0F, 0x0000);               /* RGB interface polarity             */

    /* Power On sequence -----------------------------------------------------*/
    wr_reg(0x10, 0x0000);               /* Reset Power Control 1              */
    wr_reg(0x11, 0x0000);               /* Reset Power Control 2              */
    wr_reg(0x12, 0x0000);               /* Reset Power Control 3              */
    wr_reg(0x13, 0x0000);               /* Reset Power Control 4              */
    delay(20);                          /* Discharge cap power voltage (200ms)*/
    wr_reg(0x10, 0x12B0);               /* SAP, BT[3:0], AP, DSTB, SLP, STB   */
    wr_reg(0x11, 0x0007);               /* DC1[2:0], DC0[2:0], VC[2:0]        */
    delay(5);                           /* Delay 50 ms                        */
    wr_reg(0x12, 0x01BD);               /* VREG1OUT voltage                   */
    delay(5);                           /* Delay 50 ms                        */
    wr_reg(0x13, 0x1400);               /* VDV[4:0] for VCOM amplitude        */
    wr_reg(0x29, 0x000E);               /* VCM[4:0] for VCOMH                 */
    delay(5);                           /* Delay 50 ms                        */
    wr_reg(0x20, 0x0000);               /* GRAM horizontal Address            */
    wr_reg(0x21, 0x0000);               /* GRAM Vertical Address              */

    wr_reg(0x30, 0x0B0D);
    wr_reg(0x31, 0x1923);
    wr_reg(0x32, 0x1C26);
    wr_reg(0x33, 0x261C);
    wr_reg(0x34, 0x2419);
    wr_reg(0x35, 0x0D0B);
    wr_reg(0x36, 0x1006);
    wr_reg(0x37, 0x0610);
    wr_reg(0x38, 0x0706);
    wr_reg(0x39, 0x0304);
    wr_reg(0x3A, 0x0E05);
    wr_reg(0x3B, 0x0E01);
    wr_reg(0x3C, 0x010E);
    wr_reg(0x3D, 0x050E);
    wr_reg(0x3E, 0x0403);
    wr_reg(0x3F, 0x0607);

    /* Set GRAM area ---------------------------------------------------------*/
    wr_reg(0x50, 0x0000);               /* Horizontal GRAM Start Address      */
    wr_reg(0x51, (_height-1));           /* Horizontal GRAM End   Address      */
    wr_reg(0x52, 0x0000);               /* Vertical   GRAM Start Address      */
    wr_reg(0x53, (_width-1));            /* Vertical   GRAM End   Address      */

    if (_orientLandscape ^ _orientRotate180)
        wr_reg(0x60, 0x2700);
    else
        wr_reg(0x60, 0xA700);

    wr_reg(0x61, 0x0001);               /* NDL,VLE, REV                       */
    wr_reg(0x6A, 0x0000);               /* Set scrolling line                 */

    /* Partial Display Control -----------------------------------------------*/
    wr_reg(0x80, 0x0000);
    wr_reg(0x81, 0x0000);
    wr_reg(0x82, 0x0000);
    wr_reg(0x83, 0x0000);
    wr_reg(0x84, 0x0000);
    wr_reg(0x85, 0x0000);

    /* Panel Control ---------------------------------------------------------*/
    wr_reg(0x90, 0x0010);
    wr_reg(0x92, 0x0000);
    wr_reg(0x93, 0x0003);
    wr_reg(0x95, 0x0110);
    wr_reg(0x97, 0x0000);
    wr_reg(0x98, 0x0000);

    /* Set GRAM write direction
       I/D=11 (Horizontal : increment, Vertical : increment)                  */
    if (_orientLandscape)
    /* AM=1   (address is updated in vertical writing direction)              */
      wr_reg(0x03, 0x1038);
    else
    /* AM=0   (address is updated in horizontal writing direction)            */
      wr_reg(0x03, 0x1030);

    wr_reg(0x07, 0x0137);               /* 262K color and display ON          */
  }
}


/*******************************************************************************
* Set draw window region                                                       *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        window width in pixel                            *
*                   h:        window height in pixels                          *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_SetWindow (unsigned int x, unsigned int y, unsigned int w, unsigned int h) 
{
  if (_orientLandscape)
  {
    wr_reg(0x50, y);                    /* Vertical   GRAM Start Address      */
    wr_reg(0x51, y+h-1);                /* Vertical   GRAM End   Address (-1) */
    wr_reg(0x52, x);                    /* Horizontal GRAM Start Address      */
    wr_reg(0x53, x+w-1);                /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x20, y);
    wr_reg(0x21, x);
  }
  else
  {
    wr_reg(0x50, x);                    /* Horizontal GRAM Start Address      */
    wr_reg(0x51, x+w-1);                /* Horizontal GRAM End   Address (-1) */
    wr_reg(0x52, y);                    /* Vertical   GRAM Start Address      */
    wr_reg(0x53, y+h-1);                /* Vertical   GRAM End   Address (-1) */
    wr_reg(0x20, x);
    wr_reg(0x21, y);
  }
}


/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_WindowMax (void) 
{
  GLCD_SetWindow (0, 0, _width, _height);
}


/*******************************************************************************
* Draw a pixel in foreground color                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_PutPixel (unsigned int x, unsigned int y) 
{
  if (_orientLandscape)
  {
    wr_reg(0x20, y);
    wr_reg(0x21, x);
  }
  else
  {
    wr_reg(0x20, x);
    wr_reg(0x21, y);
  }

  wr_cmd(0x22);
  wr_dat(Color[TXT_COLOR]);
}


/*******************************************************************************
* Draw a pixel in selected color                                             *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   color:    color                                            *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_PutPixelColor (unsigned int x, unsigned int y, unsigned short color) 
{
  if (_orientLandscape)
  {
    wr_reg(0x20, y);
    wr_reg(0x21, x);
  }
  else
  {
    wr_reg(0x20, x);
    wr_reg(0x21, y);
  }

  wr_cmd(0x22);
  wr_dat(color);
}


/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetTextColor (unsigned short color) {

  Color[TXT_COLOR] = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_SetBackColor (unsigned short color) 
{
  Color[BG_COLOR] = color;
}

/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Clear (unsigned short color) 
{
  unsigned int i;

  GLCD_WindowMax();
  wr_cmd(0x22);
  wr_dat_start();

  for(i = 0; i < (_width * _height); i++)
    wr_dat_only(color);
  wr_dat_stop();
}

static sGLCDFont *_fntPtrIterate = NULL;
void GLCD_InitFontList(void)
{
  _fntPtrIterate = (sGLCDFont *)glcdFonts;
}

char *GLCD_IterNextFont(void)
{
  char *fntName = NULL;
  if (_fntPtrIterate != NULL)
  {
    fntName = (char *)_fntPtrIterate->name;
    _fntPtrIterate++;
  }
  
  if (_fntPtrIterate->baseAdr == NULL)
    _fntPtrIterate = NULL;
  
  return fntName;
}

static sGLCDFont *_fntCurrent = (sGLCDFont *)glcdFonts;   // default is first in array
char *GLCD_GetCurrentFont(void)
{
  return (char *)_fntCurrent->name;
}

static int _col = 0, _row = 0;

int GLCD_SetCurentFont(char *name)
{
  if (name != NULL)
  {
    sGLCDFont *fnt;
    for (fnt = (sGLCDFont *)glcdFonts; fnt->baseAdr != NULL; fnt++)
    {
      if (strcmp(name, fnt->name) == 0)
      {
        _fntCurrent = fnt;
        break;
      }
    }
  
    if (fnt->baseAdr == NULL)     // unknown font
      return 0;                   // error state
  }

  if ((_fntCurrent->width * _col) >= _width)
    _col = _width / _fntCurrent->width;

  if ((_fntCurrent->height * _row) >= _height)
    _row = _height / _fntCurrent->height;
  
  return 1;
}

int GLCD_GetRow(void)
{
  return _row;
}

int GLCD_GetCol(void)
{
  return _col;
}

int GLCD_GetMaxRows(void)
{
  return _height / _fntCurrent->height;
}

int GLCD_ClearLine(int row)
{
  if ((_row * _fntCurrent->height) < _height)
  {
    GLCD_Box(0, row * _fntCurrent->height, _width, _fntCurrent->height, Color[BG_COLOR]);
    return 1;
  }
  else
    return 0;
}

int GLCD_GotoXY(int col, int row)
{
  _col = col;
  _row = row;
  
  return 1;
}

void GLCD_PutChar(int c)
{
  switch(c)
  {
    case '\r':
      _col = 0;
      break;
    case '\b':
      if (_col > 0)
        _col--;
      break;
    case '\n':
      // nothing todo = discard
      break;
    default:
      GLCD_DrawCharCR(_col, _row, c);
      _col++;
      
      if (_col >= (_width / _fntCurrent->width))
      {
        _col--;        // overwrite last char
      }
      break;
  }
}

void GLCD_ClearScreen(void)
{
  GLCD_Clear(Color[BG_COLOR]);
}

/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      col:        horizontal char position                              *
*                   row:        vertical char position                                *
*                   c:        character                                        *
*   Return:                                                                    *
*******************************************************************************/

int GLCD_DrawCharCR(unsigned int col, unsigned int row, unsigned char c)
{
  return GLCD_DrawCharXY(col * _fntCurrent->width, row * _fntCurrent->height, c);
}


/*******************************************************************************
* Draw character on given position                                             *
*   Parameter:      x:        horizontal position                              *
*                   z:        vertical position                                *
*                   c:        character                                        *
*   Return:                                                                    *
*******************************************************************************/

int GLCD_DrawCharXY(unsigned int x, unsigned int y, unsigned char c)
{
  unsigned int i, j, pixs;
  unsigned short *up = _fntCurrent->baseAdr;
  unsigned int baseMask = 0x0001;
  
  if ((c < _fntCurrent->offsetChar) || (c >= 128))
    return 0;
  
  if ((x > _width) || (y > _height))
    return 0;

  if (_fntCurrent->width <= 8)
    baseMask = 0x0080;                  // saved beginning MSB from 8-bit value
  else
  {
    if (_fntCurrent->width <= 16)
    {
      if (_fntCurrent->msbFirst)
        baseMask = 0x8000;         // saved beginning MSB from 16-bit value
      else
        baseMask = 0x0001;
    }
  }

  c -= _fntCurrent->offsetChar;
  
  GLCD_SetWindow(x, y, _fntCurrent->width, _fntCurrent->height);

  wr_cmd(0x22);
  wr_dat_start();

  up += c * _fntCurrent->height;
  for (j = 0; j < _fntCurrent->height; j++)
  {
    unsigned int m = baseMask;
    
    pixs = *(unsigned short *)up;
    up ++;

    for (i = 0; i < _fntCurrent->width; i++) 
    {
      wr_dat_only (Color[(pixs & m) ? 1 : 0]);         // test mask bit
      
      if (_fntCurrent->msbFirst)
        m >>= 1;
      else
        m <<= 1;
    }
  }

  wr_dat_stop();
  
  return 1;
}


void GLCD_DisplayString (unsigned char *s)
{
  while (*s) 
    GLCD_PutChar(*s++);
}


void GLCD_DisplayStringCR (unsigned int col, unsigned int row, unsigned char *s)
{
  GLCD_GotoXY(col, row);
  
  GLCD_DisplayString(s);
}


/*******************************************************************************
* Draw bargraph                                                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        maximum width of bargraph (in pixels)            *
*                   h:        bargraph height                                  *
*                   val:      value of active bargraph (in 1/1024)             *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bargraph (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int val)
{
  int i,j;

  val = (val * w) >> 10;                /* Scale value                        */
  GLCD_SetWindow(x, y, w, h);
  wr_cmd(0x22);
  wr_dat_start();
  for (i = 0; i < h; i++) 
  {
    for (j = 0; j <= w-1; j++) 
    {
      if(j >= val) 
      {
        wr_dat_only(Color[BG_COLOR]);
      } 
      else 
      {
        wr_dat_only(Color[TXT_COLOR]);
      }
    }
  }
  wr_dat_stop();
}


/*******************************************************************************
* Display graphical bitmap image at position x horizontally and y vertically   *
* (This function is optimized for 16 bits per pixel format, it has to be       *
*  adapted for any other bits per pixel format)                                *
*   Parameter:      x:        horizontal position                              *
*                   y:        vertical position                                *
*                   w:        width of bitmap                                  *
*                   h:        height of bitmap                                 *
*                   bitmap:   address at which the bitmap data resides         *
*   Return:                                                                    *
*******************************************************************************/

void GLCD_Bitmap (unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned char *bitmap)
{
  int i, j;
  unsigned short *bitmap_ptr = (unsigned short *)bitmap;

  GLCD_SetWindow (x, y, w, h);

  wr_cmd(0x22);
  wr_dat_start();
  for (i = (h-1)*w; i > -1; i -= w) 
  {
    for (j = 0; j < w; j++) 
    {
      wr_dat_only (bitmap_ptr[i+j]);
    }
  }
  wr_dat_stop();
}


/*******************************************************************************
* Write a command to the LCD controller                                        *
*   Parameter:      cmd:      command to write to the LCD                      *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_WrCmd (unsigned char cmd) 
{
  wr_cmd (cmd);
}


/*******************************************************************************
* Write a value into LCD controller register                                   *
*   Parameter:      reg:      lcd register address                             *
*                   val:      value to write into reg                          *
*   Return:                                                                    *
*******************************************************************************/
void GLCD_WrReg (unsigned char reg, unsigned short val) 
{
  wr_reg (reg, val);
}

void GLCD_WrDat (unsigned short dat) 
{
  wr_dat (dat);
}

/* myCode */

void GLCD_Box(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned short color)
{
	int ww = w, xx = x;
	
	GLCD_WindowMax();
//	GLCD_SetTextColor(color);
	
  if (_orientLandscape)
  {
    for(; h; h--)
    {
      x = xx;

      wr_reg(0x21, x);
      wr_reg(0x20, y);

      wr_cmd(0x22);
      for(w = ww; w; w--)
      {
        wr_dat(color);

        x++;
      }
      y++;
    }
  }
  else
  {
    for(; h; h--)
    {
      x = xx;

      wr_reg(0x20, x);
      wr_reg(0x21, y);

      wr_cmd(0x22);
      wr_dat_start();
      for(w = ww; w; w--)
      {
        wr_dat_only(color);

        x++;
      }
      wr_dat_stop();
      y++;
    }
  }
}

/// <remarks>http://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C</remarks>
void GLCD_Line(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1, unsigned short color)
{
  if (y0 == y1)     // vodorovna cara ?
  {
    if (x0 > x1)    // obracene X souradnice
    {
      unsigned int u = x0;    // swap values
      x0 = x1;
      x1 = u;
    }

    if (x1 > _width)
      x1 = _width;

    if (_orientLandscape)
    { 
      wr_reg(0x21, x0);
      wr_reg(0x20, y0);
    }
    else
    {
      wr_reg(0x21, y0);
      wr_reg(0x20, x0);
    }
    
    wr_cmd(0x22);
    wr_dat_start();
    for(; x0 < x1; x0++)
      wr_dat_only(color);
    wr_dat_stop();
  }
  else
  {
    if (y0 > y1)    // obracene Y souradnice
    {
      unsigned int u = y0;    // swap values
      y0 = y1;
      y1 = u;
    }
    
    if (y1 > _height)
      y1 = _height;

    {
      int dx = (x0 < x1) ? (x1 - x0) : (x0 - x1), sx = (x0 < x1) ? 1 : -1;
      int dy = (y0 < y1) ? (y1 - y0) : (y0 - y1), sy = (y0 < y1) ? 1 : -1;
      int err = ((dx > dy) ? dx : -dy) / 2, e2;

      for (; ; )
      {
        if ((x0 == x1) && (y0 == y1))
          break;

        GLCD_PutPixelColor(x0, y0, color);

        e2 = err;
        if (e2 > -dx)
        {
          err -= dy; x0 += sx;
        }

        if (e2 < dy)
        {
          err += dx; y0 += sy;
        }
      }
    }
  }
}

// http://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C
void GLCD_Circle(unsigned int x0, unsigned int y0, unsigned int radius, unsigned int color)
{
  int f = 1 - radius;
  int ddF_x = 0;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = radius;

  GLCD_PutPixelColor(x0, y0 + radius, color);
  GLCD_PutPixelColor(x0, y0 - radius, color);
  GLCD_PutPixelColor(x0 + radius, y0, color);
  GLCD_PutPixelColor(x0 - radius, y0, color);

  while(x < y) 
  {
    if(f >= 0) 
    {
        y--;
        ddF_y += 2;
        f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x + 1;    
    
    GLCD_PutPixelColor(x0 + x, y0 + y, color);
    GLCD_PutPixelColor(x0 - x, y0 + y, color);
    GLCD_PutPixelColor(x0 + x, y0 - y, color);
    GLCD_PutPixelColor(x0 - x, y0 - y, color);
    GLCD_PutPixelColor(x0 + y, y0 + x, color);
    GLCD_PutPixelColor(x0 - y, y0 + x, color);
    GLCD_PutPixelColor(x0 + y, y0 - x, color);
    GLCD_PutPixelColor(x0 - y, y0 - x, color);
  }
}

void GLCD_CircleFill(unsigned int x0, unsigned int y0, unsigned int radius, unsigned int color)
{
  int f = 1 - radius;
  int ddF_x = 0;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = (int)radius;
  int t1, t2;

//  GLCD_Line(x0, y0 + radius, x0, y0 - radius, color);
  GLCD_PutPixelColor(x0, y0 + radius, color);
  t1 = y0 - radius;
  GLCD_PutPixelColor(x0, (t1 > 0) ? t1 : 0, color);
  t1 = x0 - radius;
  GLCD_Line(x0 + radius, y0, (t1 > 0) ? t1 : 0, y0, color);

  while(x < y) 
  {
    if(f >= 0) 
    {
        y--;
        ddF_y += 2;
        f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x + 1;    

    t1 = x0 - x;
    t2 = y0 - y;
    GLCD_Line((t1 > 0) ? t1 : 0, y0 + y, x0 + x, y0 + y, color);
    GLCD_Line((t1 > 0) ? t1 : 0, (t2 > 0) ? t2 : 0, x0 + x, (t2 > 0) ? t2 : 0, color);
    t1 = x0 - y;
    t2 = y0 - x;
    GLCD_Line((t1 > 0) ? t1 : 0, y0 + x, x0 + y, y0 + x, color);
    GLCD_Line((t1 > 0) ? t1 : 0, (t2 > 0) ? t2 : 0, x0 + y, (t2 > 0) ? t2 : 0, color);
    
/*
    GLCD_PutPixelColor(x0 + x, y0 + y, color);
    GLCD_PutPixelColor(x0 - x, y0 + y, color);
    GLCD_PutPixelColor(x0 + x, y0 - y, color);
    GLCD_PutPixelColor(x0 - x, y0 - y, color);
    GLCD_PutPixelColor(x0 + y, y0 + x, color);
    GLCD_PutPixelColor(x0 - y, y0 + x, color);
    GLCD_PutPixelColor(x0 + y, y0 - x, color);
    GLCD_PutPixelColor(x0 - y, y0 - x, color);
*/
  }
}

/******************************************************************************/
