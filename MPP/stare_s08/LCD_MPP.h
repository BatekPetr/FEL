/*
 * LCD_MPP.h
 *
 *  Created on: Apr 27, 2014
 *      Author: weiss_000
 */

#ifndef LCD_MPP_H_
#define LCD_MPP_H_

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include inherited components */

#include "Cpu.h"

/*
  Inicialization of HW
  
  Internally use LCD and TOD block

  Don't use these blocks in Processor Expert
*/
void LCD_Init(void);

// definition of position symbols on LCD glass
// this value-pair can be directly use in function SegmentOn / Off
#define SYMBOL_MO   1,1 
#define SYMBOL_P0   2,1 
#define SYMBOL_P1   2,2
#define SYMBOL_P2   3,2
#define SYMBOL_P3   3,1
#define SYMBOL_P4   4,1
#define SYMBOL_TU   5,1
#define SYMBOL_WE   9,1
#define SYMBOL_COL  9,2
#define SYMBOL_TH   10,1
#define SYMBOL_FR   14,1
#define SYMBOL_FLAME 15,1 
#define SYMBOL_SA   15,2
#define SYMBOL_SNOW 16,1
#define SYMBOL_SU   16,2
#define SYMBOL_AM   20,1
#define SYMBOL_PM   20,2
#define SYMBOL_FLTR 21,1
#define SYMBOL_HOLD 21,2

// definition for segments, internally used by LCD_Show.. functions
#define SEGMENT_T1  1,2

#define SEGMENT_1A  5,2
#define SEGMENT_1B  6,1
#define SEGMENT_1C  7,1
#define SEGMENT_1D  8,1
#define SEGMENT_1E  8,2
#define SEGMENT_1F  6,2
#define SEGMENT_1G  7,2

#define SEGMENT_2A  10,2
#define SEGMENT_2B  11,1
#define SEGMENT_2C  12,1
#define SEGMENT_2D  13,1
#define SEGMENT_2E  13,2
#define SEGMENT_2F  11,2
#define SEGMENT_2G  12,2

#define SEGMENT_3A  14,2
#define SEGMENT_3B  17,1
#define SEGMENT_3C  18,1
#define SEGMENT_3D  19,1
#define SEGMENT_3E  19,2
#define SEGMENT_3F  17,2
#define SEGMENT_3G  18,2

#define SEGMENT_4A  25,1
#define SEGMENT_4B  25,2
#define SEGMENT_4C  23,2
#define SEGMENT_4D  22,1
#define SEGMENT_4E  23,1
#define SEGMENT_4F  24,1
#define SEGMENT_4G  24,2

#define SEGMENT_5A  28,1
#define SEGMENT_5B  28,2
#define SEGMENT_5C  26,2
#define SEGMENT_5D  22,2
#define SEGMENT_5E  26,1
#define SEGMENT_5F  27,1
#define SEGMENT_5G  27,2

// ON or OFF selected segment (defined by IO pin and Common pin) = use predefined constants
void LCD_SegmentOn(byte lcdpin, byte compin);
void LCD_SegmentOff(byte lcdpin, byte compin);

// show value 0 - 9 as 7-seg number on position 0 - 6 (position 0 can show value 0 or 1)
bool LCD_ShowNumber(byte pos, byte val);
// show value 0 - 26 matches ASCII character A - Z as 7-seg symbol on position 1 - 6 (position 0 not used - single segment)
bool LCD_ShowCharacter(byte pos, byte code);

void LCD_AllSymbolsOn(void);
void LCD_AllSymbolsOff(void);

// low level access to 7-seg parts, position valid 1 - 5, bits is bit representation of 7-seg
bool LCD_ShowSegments(byte pos, byte bits);

#endif /* LCD_MPP_H_ */
