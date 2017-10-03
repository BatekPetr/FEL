/*
 * LCD_MPP.c
 *
 *  Created on: Apr 27, 2014
 *      Author: weiss_000
 */

#include "LCD_MPP.h"

/*
 * Initially setting of LCD
 * 
 * !!! Enable external clock with "crystal" source - register ICSC2
 * !!! Source clock for LCD is "external" 
 */
void LCD_Init(void)
{
  __DI();                              /* Disable interrupts */
  
//  TOD_Init();
//  static void TOD_Init(void)
  {
    /* TODC: TODEN=0 */
    clrReg8Bits(TODC, 0x80U);
    //  /* TODC: TODCLKS=0,TODR=1,TODCLKEN=1,TODPS=6 */
    //  clrSetReg8Bits(TODC, 0x61U, 0x1EU);
    // TODEN = 0, TODCLKS = 10, TODR = 1, TODCLKEN = 1, TODPS = 1 .... = 0101 1001 
    setReg8(TODC, 0x59U);
    /* TODM: TODM=0,MQSEC=0 */
    setReg8(TODM, 0x00U);
    /* TODSC: QSECF=1,SECF=1,MTCHF=1,QSECIE=0,SECIE=0,MTCHIE=0,MTCHEN=0,MTCHWC=0 */
    setReg8(TODSC, 0xE0U);
    /* TODC: TODEN=1 */
    setReg8Bits(TODC, 0x80U);
  }

//  _ICSC2.Bits.ERCLKEN = 1;
//  _ICSC2.Bits.EREFS = 1;
  
  /* LCDC0: LCDEN=0 */
  clrReg8Bits(LCDC0, 0x80U);  // !!
  /* LCDC0: SOURCE=0 */
//  clrReg8Bits(LCDC0, 0x40U);            
  /* LCDC0: SOURCE=1 */ // = Alternate clock pg 227 in RM / chapter 13.3.1
// zbytecne, prepise se v nastaveni LCDC0 o 8 radku nize !!  setReg8Bits(LCDC0, 0x40U);            
  /* LCDRVC: RVEN=1,??=0,??=0,??=0,RVTRIM3=0,RVTRIM2=0,RVTRIM1=0,RVTRIM0=0 */
  setReg8(LCDRVC, 0x80U);               
  /* LCDSUPPLY: CPSEL=1,HREFSEL=0,LADJ1=0,LADJ0=0,??=0,BBYPASS=0,VSUPPLY1=1,VSUPPLY0=1 */
  setReg8(LCDSUPPLY, 0x83U);            
  /* LCDC1: LCDIEN=0,??=0,??=0,??=0,??=0,FCDEN=0,LCDWAI=0,LCDSTP=0 */
  setReg8(LCDC1, 0x00U);                
// pozor, mazalo alt. clock bit !!  /* LCDC0: LCDEN=0,SOURCE=0,LCLK2=0,LCLK1=0,LCLK0=0,DUTY2=0,DUTY1=0,DUTY0=1 */
//  setReg8(LCDC0, 0x01U);                
  /* LCDC0: LCDEN=0,SOURCE=1,LCLK2=0,LCLK1=0,LCLK0=0,DUTY2=0,DUTY1=0,DUTY0=1 */
  setReg8(LCDC0, 0x41U);  
  /* LCDBCTL: BLINK=0,ALT=0,BLANK=0,??=0,BMODE=0,BRATE2=0,BRATE1=0,BRATE0=0 */
  setReg8(LCDBCTL, 0x00U);              
  /* LCDPEN0: PEN7=1,PEN6=1,PEN5=1,PEN4=1,PEN3=1,PEN2=1,PEN1=1,PEN0=1 */
  setReg8(LCDPEN0, 0xFFU);              
  /* LCDBPEN0: BPEN7=0,BPEN6=0,BPEN5=0,BPEN4=0,BPEN3=0,BPEN2=0,BPEN1=0,BPEN0=0 */
  setReg8(LCDBPEN0, 0x00U);             
  /* LCDPEN1: PEN15=1,PEN14=1,PEN13=1,PEN12=0,PEN11=0,PEN10=0,PEN9=0,PEN8=0 */
  setReg8(LCDPEN1, 0xE0U);              
  /* LCDBPEN1: BPEN15=0,BPEN14=0,BPEN13=0,BPEN12=0,BPEN11=0,BPEN10=0,BPEN9=0,BPEN8=0 */
  setReg8(LCDBPEN1, 0x00U);             
  /* LCDPEN2: PEN23=1,PEN22=1,PEN21=1,PEN20=1,PEN19=1,PEN18=1,PEN17=1,PEN16=1 */
  setReg8(LCDPEN2, 0xFFU);              
  /* LCDBPEN2: BPEN23=0,BPEN22=0,BPEN21=0,BPEN20=0,BPEN19=0,BPEN18=0,BPEN17=0,BPEN16=0 */
  setReg8(LCDBPEN2, 0x00U);             
  /* LCDPEN3: PEN31=0,PEN30=1,PEN29=1,PEN28=1,PEN27=1,PEN26=1,PEN25=1,PEN24=1 */
  setReg8(LCDPEN3, 0x7FU);              
  /* LCDBPEN3: BPEN31=0,BPEN30=0,BPEN29=0,BPEN28=0,BPEN27=0,BPEN26=0,BPEN25=0,BPEN24=0 */
  setReg8(LCDBPEN3, 0x00U);             
  /* LCDPEN4: PEN39=1,PEN38=1,PEN37=0,PEN36=0,PEN35=0,PEN34=0,PEN33=0,PEN32=0 */
  setReg8(LCDPEN4, 0xC0U);              
  /* LCDBPEN4: BPEN39=0,BPEN38=0,BPEN37=0,BPEN36=0,BPEN35=0,BPEN34=0,BPEN33=0,BPEN32=0 */
  setReg8(LCDBPEN4, 0x00U);             
  /* LCDPEN5: ??=0,??=0,??=0,??=0,PEN43=0,PEN42=0,PEN41=1,PEN40=1 */
  setReg8(LCDPEN5, 0x03U);              
  /* LCDBPEN5: ??=0,??=0,??=0,??=0,BPEN43=0,BPEN42=0,BPEN41=1,BPEN40=1 */
  setReg8(LCDBPEN5, 0x03U);             
  /* LCDWF40: BPHLCD40=0,BPGLCD40=0,BPFLCD40=0,BPELCD40=0,BPDLCD40=0,BPCLCD40=0,BPBLCD40=0,BPALCD40=1 */
  setReg8(LCDWF40, 0x01U);              
  /* LCDWF41: BPHLCD41=0,BPGLCD41=0,BPFLCD41=0,BPELCD41=0,BPDLCD41=0,BPCLCD41=0,BPBLCD41=1,BPALCD41=0 */
  setReg8(LCDWF41, 0x02U);              
  /* LCDS: LCDIF=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
  setReg8(LCDS, 0x80U);                 
  /* LCDC0: LCDEN=1 */
  setReg8Bits(LCDC0, 0x80U);     
  
  __EI();                              /* Enable interrupts */
}

/*
 * Re-coding pins from CPU to LCD - see schematics
 */
static const byte DispPin2LcdPin[] =
{
    0,    // dummy, pins on LCD indexed from 1, LW registers from 0
    0,    // LCD pin 1, LCDWF0
    1,    // LCD pin 2, LCDWF1
    2,    // LCD pin 3, LCDWF2
    3,    // LCD pin 4, LCDWF3
    4,    // LCD pin 5, LCDWF4
    5,    // LCD pin 6, LCDWF5
    6,    // LCD pin 7, LCDWF6
    7,    // LCD pin 8, LCDWF7
    13,    // LCD pin 9, LCDWF13
    14,    // LCD pin 10, LCDWF14
    15,    // LCD pin 11, LCDWF15
    16,    // LCD pin 12, LCDWF16
    17,    // LCD pin 13, LCDWF17
    18,    // LCD pin 14, LCDWF18
    19,    // LCD pin 15, LCDWF19
    20,    // LCD pin 16, LCDWF20
    21,    // LCD pin 17, LCDWF21
    22,    // LCD pin 18, LCDWF22
    23,    // LCD pin 19, LCDWF23
    24,    // LCD pin 20, LCDWF24
    25,    // LCD pin 21, LCDWF25
    26,    // LCD pin 22, LCDWF26
    27,    // LCD pin 23, LCDWF27
    28,    // LCD pin 24, LCDWF28
    29,    // LCD pin 25, LCDWF29
    30,    // LCD pin 26, LCDWF30
    38,    // LCD pin 27, LCDWF38
    39,    // LCD pin 28, LCDWF39
//    1,    // LCD pin 29, COM1 - PIN40
//    1,    // LCD pin 30, COM2 - PIN41
    0     // dummy
};

/*
 * Lowlevel control of outputs - segment on
 *   - lcdpin - control register ID (internally recoded by schematics)
 *   - compin - selected output plane, GD-5360P use 2 planes, e.g. valid values are 1 or 2 
 * 
 * Remarks:
 *   definitions are created as pair of 2 values for these parameters
 */
void LCD_SegmentOn(byte lcdpin, byte compin)
{
  *(LCDWF_ARR + DispPin2LcdPin[lcdpin]) |= (1 << (compin - 1));
}

/*
 * Lowlevel control of outputs - segment off
 *   - lcdpin - control register ID (internally recoded by schematics)
 *   - compin - selected output plane, GD-5360P use 2 planes, e.g. valid values are 1 or 2 
 * 
 * Remarks:
 *   definitions are created as pair of 2 values for these parameters
 */
void LCD_SegmentOff(byte lcdpin, byte compin)
{
  *(LCDWF_ARR + DispPin2LcdPin[lcdpin]) &= ~(1 << (compin - 1));
}

// Order of segments in character: 0GFE DCBA
static const byte segments_numbers[] =
{
    0x3f,   // 0 - 0011 1111
    0x06,   // 1 - 0000 0110
    0x5b,   // 2 - 0101 1011
    0x4f,   // 3 - 0100 1111
    0x66,   // 4 - 0110 0110
    0x6d,   // 5 - 0110 1101
    0x7d,   // 6 - 0111 1101
    0x07,   // 7 - 0000 0111
    0x7f,   // 8 - 0111 1111
    0x6f    // 9 - 0110 1111
};

static const byte segments_chars[] =
{
    0x77,   // A - 0111 0111
    0x7c,   // B - 0111 1100
    0x39,   // C - 0011 1001
    0x5e,   // D - 0101 1110
    0x79,   // E - 0111 1001
    0x71,   // F - 0111 0001
    0x3d,   // G - 0011 1101
    0x76,   // H - 0111 0110
    0x40,   // I - 0100 0110
    0x1e,   // J - 0001 1110
    0x76,   // K - 0111 0110
    0x38,   // L - 0011 1000
    0x40,   // M - 0100 0000
    0x40,   // N - 0100 0000
    0x5c,   // O - 0101 1100
    0x73,   // P - 0111 0011
    0x40,   // Q - 0100 0000
    0x50,   // R - 0101 0000
    0x6d,   // S - 0110 1101
    0x07,   // T - 0000 0111
    0x3e,   // U - 0011 1110
    0x1c,   // V - 0001 1100
    0x40,   // W - 0100 0000
    0x40,   // X - 0100 0000
    0x66,   // Y - 0110 0110
    0x5b    // Z - 0101 1011
};

/*
 * Bit definition for special characters/symbols
 */
#define SEGMENTS_NONE   0x00
#define SEGMENTS_DASH   0x40

/*
 * Show segment combination on 7-segment position
 * 
 * Valid parameters based od LCD layout:
 *   pos - position 1 - 5
 *   bits - bit combination of character definition
 *   
 * Return:
 *   FALSE - invalid parameters
 *   TRUE - processed OK  
 */
bool LCD_ShowSegments(byte pos, byte bits)
{
  bool bbOn;
  byte bMask = 1;
  
  bbOn = (bits & bMask) != 0;
  switch(pos)
  {
    case 1: if (bbOn) LCD_SegmentOn(SEGMENT_1A); else LCD_SegmentOff(SEGMENT_1A); break;
    case 2: if (bbOn) LCD_SegmentOn(SEGMENT_2A); else LCD_SegmentOff(SEGMENT_2A); break;
    case 3: if (bbOn) LCD_SegmentOn(SEGMENT_3A); else LCD_SegmentOff(SEGMENT_3A); break;
    case 4: if (bbOn) LCD_SegmentOn(SEGMENT_4A); else LCD_SegmentOff(SEGMENT_4A); break;
    case 5: if (bbOn) LCD_SegmentOn(SEGMENT_5A); else LCD_SegmentOff(SEGMENT_5A); break;
  }
  
  bMask <<= 1;
  bbOn = (bits & bMask) != 0;
  switch(pos)
  {
    case 1: if (bbOn) LCD_SegmentOn(SEGMENT_1B); else LCD_SegmentOff(SEGMENT_1B); break;
    case 2: if (bbOn) LCD_SegmentOn(SEGMENT_2B); else LCD_SegmentOff(SEGMENT_2B); break;
    case 3: if (bbOn) LCD_SegmentOn(SEGMENT_3B); else LCD_SegmentOff(SEGMENT_3B); break;
    case 4: if (bbOn) LCD_SegmentOn(SEGMENT_4B); else LCD_SegmentOff(SEGMENT_4B); break;
    case 5: if (bbOn) LCD_SegmentOn(SEGMENT_5B); else LCD_SegmentOff(SEGMENT_5B); break;
  }
  
  bMask <<= 1;
  bbOn = (bits & bMask) != 0;
  switch(pos)
  {
    case 1: if (bbOn) LCD_SegmentOn(SEGMENT_1C); else LCD_SegmentOff(SEGMENT_1C); break;
    case 2: if (bbOn) LCD_SegmentOn(SEGMENT_2C); else LCD_SegmentOff(SEGMENT_2C); break;
    case 3: if (bbOn) LCD_SegmentOn(SEGMENT_3C); else LCD_SegmentOff(SEGMENT_3C); break;
    case 4: if (bbOn) LCD_SegmentOn(SEGMENT_4C); else LCD_SegmentOff(SEGMENT_4C); break;
    case 5: if (bbOn) LCD_SegmentOn(SEGMENT_5C); else LCD_SegmentOff(SEGMENT_5C); break;
  }
  
  bMask <<= 1;
  bbOn = (bits & bMask) != 0;
  switch(pos)
  {
    case 1: if (bbOn) LCD_SegmentOn(SEGMENT_1D); else LCD_SegmentOff(SEGMENT_1D); break;
    case 2: if (bbOn) LCD_SegmentOn(SEGMENT_2D); else LCD_SegmentOff(SEGMENT_2D); break;
    case 3: if (bbOn) LCD_SegmentOn(SEGMENT_3D); else LCD_SegmentOff(SEGMENT_3D); break;
    case 4: if (bbOn) LCD_SegmentOn(SEGMENT_4D); else LCD_SegmentOff(SEGMENT_4D); break;
    case 5: if (bbOn) LCD_SegmentOn(SEGMENT_5D); else LCD_SegmentOff(SEGMENT_5D); break;
  }
  
  bMask <<= 1;
  bbOn = (bits & bMask) != 0;
  switch(pos)
  {
    case 1: if (bbOn) LCD_SegmentOn(SEGMENT_1E); else LCD_SegmentOff(SEGMENT_1E); break;
    case 2: if (bbOn) LCD_SegmentOn(SEGMENT_2E); else LCD_SegmentOff(SEGMENT_2E); break;
    case 3: if (bbOn) LCD_SegmentOn(SEGMENT_3E); else LCD_SegmentOff(SEGMENT_3E); break;
    case 4: if (bbOn) LCD_SegmentOn(SEGMENT_4E); else LCD_SegmentOff(SEGMENT_4E); break;
    case 5: if (bbOn) LCD_SegmentOn(SEGMENT_5E); else LCD_SegmentOff(SEGMENT_5E); break;
  }
  
  bMask <<= 1;
  bbOn = (bits & bMask) != 0;
  switch(pos)
  {
    case 1: if (bbOn) LCD_SegmentOn(SEGMENT_1F); else LCD_SegmentOff(SEGMENT_1F); break;
    case 2: if (bbOn) LCD_SegmentOn(SEGMENT_2F); else LCD_SegmentOff(SEGMENT_2F); break;
    case 3: if (bbOn) LCD_SegmentOn(SEGMENT_3F); else LCD_SegmentOff(SEGMENT_3F); break;
    case 4: if (bbOn) LCD_SegmentOn(SEGMENT_4F); else LCD_SegmentOff(SEGMENT_4F); break;
    case 5: if (bbOn) LCD_SegmentOn(SEGMENT_5F); else LCD_SegmentOff(SEGMENT_5F); break;
  }
  
  bMask <<= 1;
  bbOn = (bits & bMask) != 0;
  switch(pos)
  {
    case 1: if (bbOn) LCD_SegmentOn(SEGMENT_1G); else LCD_SegmentOff(SEGMENT_1G); break;
    case 2: if (bbOn) LCD_SegmentOn(SEGMENT_2G); else LCD_SegmentOff(SEGMENT_2G); break;
    case 3: if (bbOn) LCD_SegmentOn(SEGMENT_3G); else LCD_SegmentOff(SEGMENT_3G); break;
    case 4: if (bbOn) LCD_SegmentOn(SEGMENT_4G); else LCD_SegmentOff(SEGMENT_4G); break;
    case 5: if (bbOn) LCD_SegmentOn(SEGMENT_5G); else LCD_SegmentOff(SEGMENT_5G); break;
  }
  
  return TRUE;
}

/*
 * Try to show number on 7-segment position
 * 
 * Valid parameters based od LCD layout:
 *   pos - position 0 - 5 (position 0 = symbol T1 - can show only 0 or 1)
 *   val - value 0 - 9 (except position 0)
 *   
 * Return:
 *   FALSE - invalid parameters
 *   TRUE - processed OK  
 */
bool LCD_ShowNumber(byte pos, byte val)
{
  byte bDefine = 0;
  
  if (pos > 6)
    return FALSE;
  
  if (pos == 0)
  {
    if (val == 0)
    {
      LCD_SegmentOff(SEGMENT_T1);
      return TRUE;
    }

    if (val == 1)
    {
      LCD_SegmentOn(SEGMENT_T1);
      return TRUE;
    }
    
    return FALSE;
  }
  
  if (val > 9)
    bDefine = SEGMENTS_DASH;
  else
    bDefine = segments_numbers[val];
  
  return LCD_ShowSegments(pos, bDefine);
}
  
/*
 * Try to show ASCII characters on 7-segment position
 * 
 * Valid parameters based od LCD layout:
 *   pos - position 1 - 5 (symbol T1 not used)
 *   code - character code - based on ASCII, valid 0 - 25, where 0 - 'A' and 25 - 'Z'
 *   
 * Return:
 *   FALSE - invalid parameters
 *   TRUE - processed OK  
 *   
 * Remark:
 *   some characters are not usable :-(
 */
bool LCD_ShowCharacter(byte pos, byte code)
{
  byte bDefine = 0;
  
  if ((pos < 1) || (pos > 6))
    return FALSE;
  
  if (code >= 26)    // 'Z' - 'A'
    bDefine = SEGMENTS_DASH;
  else
    bDefine = segments_chars[code];
  
  return LCD_ShowSegments(pos, bDefine);
}
  
/*
 * Light on all symbols except 7-segment numbers
 */
void LCD_AllSymbolsOn(void)
{
  LCD_SegmentOn(SYMBOL_MO); 
  LCD_SegmentOn(SYMBOL_P0); 
  LCD_SegmentOn(SYMBOL_P1); 
  LCD_SegmentOn(SYMBOL_P2); 
  LCD_SegmentOn(SYMBOL_P3); 
  LCD_SegmentOn(SYMBOL_P4); 
  LCD_SegmentOn(SYMBOL_TU); 
  LCD_SegmentOn(SYMBOL_WE); 
  LCD_SegmentOn(SYMBOL_COL); 
  LCD_SegmentOn(SYMBOL_TH); 
  LCD_SegmentOn(SYMBOL_FR); 
  LCD_SegmentOn(SYMBOL_FLAME); 
  LCD_SegmentOn(SYMBOL_SA); 
  LCD_SegmentOn(SYMBOL_SNOW); 
  LCD_SegmentOn(SYMBOL_SU); 
  LCD_SegmentOn(SYMBOL_AM); 
  LCD_SegmentOn(SYMBOL_PM); 
  LCD_SegmentOn(SYMBOL_FLTR); 
  LCD_SegmentOn(SYMBOL_HOLD); 
}

/*
 * Light off all symbols except 7-segment numbers 
 */
void LCD_AllSymbolsOff(void)
{
  LCD_SegmentOff(SYMBOL_MO); 
  LCD_SegmentOff(SYMBOL_P0); 
  LCD_SegmentOff(SYMBOL_P1); 
  LCD_SegmentOff(SYMBOL_P2); 
  LCD_SegmentOff(SYMBOL_P3); 
  LCD_SegmentOff(SYMBOL_P4); 
  LCD_SegmentOff(SYMBOL_TU); 
  LCD_SegmentOff(SYMBOL_WE); 
  LCD_SegmentOff(SYMBOL_COL); 
  LCD_SegmentOff(SYMBOL_TH); 
  LCD_SegmentOff(SYMBOL_FR); 
  LCD_SegmentOff(SYMBOL_FLAME); 
  LCD_SegmentOff(SYMBOL_SA); 
  LCD_SegmentOff(SYMBOL_SNOW); 
  LCD_SegmentOff(SYMBOL_SU); 
  LCD_SegmentOff(SYMBOL_AM); 
  LCD_SegmentOff(SYMBOL_PM); 
  LCD_SegmentOff(SYMBOL_FLTR); 
  LCD_SegmentOff(SYMBOL_HOLD); 
}
