#include "mbed_lcd_hw.h"

void MBED_LCD_init_hw(void)
{
  #ifdef MBED_LCD_USE_SPI
  if (!(RCC->APB2ENR & RCC_APB2ENR_SPI1EN))
  {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
  }
  
  SPI1->CR1 = SPI_CR1_BR_0;      // 001 = clk/4 - z APB2 (72MHz)
  SPI1->CR1 |= SPI_CR1_MSTR;// | SPI_CR1_BIDIOE;
  SPI1->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM;
  SPI1->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL; // viz. RM0008 pg. 696/1128 (rev 15)
  SPI1->CR2 = 0;

  SPI1->CR1 |= SPI_CR1_SPE;
  
  InitIOPort(GPIOA, 5, portALTOUT);
  InitIOPort(GPIOA, 7, portALTOUT);

  if (!(RCC->APB2ENR & RCC_APB2ENR_AFIOEN))
  {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2RSTR |= RCC_APB2RSTR_AFIORST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_AFIORST;
  }
  
  AFIO->MAPR &= ~ AFIO_MAPR_SPI1_REMAP;
  #else
  SET_IO_HIGH(GPIOA, 5);
  InitIOPort(GPIOA, 5, portOUTPUT);

  SET_IO_HIGH(GPIOA, 7);
  InitIOPort(GPIOA, 7, portOUTPUT);
  #endif

  SET_IO_HIGH(GPIOA, 6);
  InitIOPort(GPIOA, 6, portOUTPUT);

  SET_IO_HIGH(GPIOA, 8);
  InitIOPort(GPIOA, 8, portOUTPUT);

  SET_IO_HIGH(GPIOB, 6);
  InitIOPort(GPIOB, 6, portOUTPUT);
}

void MBED_LCD_send(byte val, bool a0)
{
#ifdef MBED_LCD_USE_SPI
  if (a0)
    MBED_LCD_A0_HI();
  else
    MBED_LCD_A0_LO();
  
  MBED_LCD_CS_LO();
  
  SPI1->DR = val;
  while(SPI1->SR & SPI_SR_BSY)
    ;
  
  MBED_LCD_CS_HI();
#else
  #define MY_SPI_WAIT 4
  {
    byte mask = 0x80u;
    uint x;

    MBED_LCD_SCK_HI();

    MBED_LCD_SCK_LO();
    if (a0)
      MBED_LCD_A0_HI();
    else
      MBED_LCD_A0_LO();
    
    for(x = 0; x < MY_SPI_WAIT; x++)
      ;

    MBED_LCD_CS_LO();
    
    while(mask)
    {
      if (val & mask)
        MBED_LCD_MOSI_HI();
      else
        MBED_LCD_MOSI_LO();

      mask >>= 1;
      
      for(x = 0; x < MY_SPI_WAIT; x++)
        ;
      MBED_LCD_SCK_HI();

      for(x = 0; x < MY_SPI_WAIT; x++)
        ;
      MBED_LCD_SCK_LO();
    }
    
    MBED_LCD_CS_HI();
    MBED_LCD_SCK_HI();
  }
#endif  
}

bool MBED_LCD_reset(void)
{
  word w, x = 0;
  
  MBED_LCD_A0_LO();
  
  MBED_LCD_RESET_LO();
  
  for(w = 0; w < 10000; w++)
    x++;
  
  MBED_LCD_RESET_HI();

  for(w = 0; w < 1000; w++)
    x++;

  return (x > 0);
}
