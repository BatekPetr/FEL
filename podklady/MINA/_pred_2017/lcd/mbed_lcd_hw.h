#ifndef _MBED_LCD_HW_H
#define _MBED_LCD_HW_H

#include "mbed.h"

#define MBED_LCD_USE_SPI

//  MOSI:D11 = PA7, nRESET:D12 = PA6, SCK:D13 = PA5, A0:D7 = PA8, nCS:D10 = PB6
void MBED_LCD_init_hw(void); 
void MBED_LCD_send(byte val, bool a0);
bool MBED_LCD_reset(void);
  
#ifdef MBED_LCD_USE_SPI
#else  // MBED_LCD_USE_SPI
#define MBED_LCD_MOSI_HI()  GPIOA->BSRR |= (1 << 7)
#define MBED_LCD_MOSI_LO()  GPIOA->BRR |= (1 << 7)

#define MBED_LCD_SCK_HI()  GPIOA->BSRR |= (1 << 5)
#define MBED_LCD_SCK_LO()  GPIOA->BRR |= (1 << 5)
#endif  // MBED_LCD_USE_SPI

#define MBED_LCD_RESET_HI()  GPIOA->BSRR |= (1 << 6)
#define MBED_LCD_RESET_LO()  GPIOA->BRR |= (1 << 6)

#define MBED_LCD_A0_HI()  GPIOA->BSRR |= (1 << 8)
#define MBED_LCD_A0_LO()  GPIOA->BRR |= (1 << 8)

#define MBED_LCD_CS_HI()  GPIOB->BSRR |= (1 << 6)
#define MBED_LCD_CS_LO()  GPIOB->BRR |= (1 << 6)

#endif // _MBED_LCD_HW_H
