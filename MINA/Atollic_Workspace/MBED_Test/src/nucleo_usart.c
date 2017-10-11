/*
 * nucleo_usart.c
 *
 *  Created on: Oct 2, 2017
 *      Author: batekp
 */

#include <nucleo_usart.h>

// USART Functions
void Usart2Init(uint32_t baudSpeed)
{
    //Set TX and RX pins to AF
    Nucleo_SetPinGPIO(GPIOA, 2, ioPortAlternatePP);
    Nucleo_SetPinGPIO(GPIOA, 3, ioPortAlternatePP);

    Nucleo_SetPinAFGPIO(GPIOA, 2, 7);
    Nucleo_SetPinAFGPIO(GPIOA, 3, 7);

    // Enable Clock to USART2
    if (!(RCC->APB1ENR & RCC_APB1ENR_USART2EN)) // neni povolen USART2
    {
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
      RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
      RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
    }

    USART2->CR1 = USART_CR1_RE | USART_CR1_TE; // staci povoleni prijmu a vysilani
    USART2->CR2 = 0; // nic specialniho
    USART2->CR3 = 0; // nic specialniho


    // vypocet BRR podle pozadovaneho baudSpeed a podle clocku
    //int over8 = USART2->CR2 & USART_CR1_OVER8; // Vymyslet to lip, pouzit OVER8 z USART_CR2
    uint sampling = (USART2->CR1 & USART_CR1_OVER8) ? 8 : 16;
    uint32_t apb1, mant, tmp, frac;
    apb1 = GetBusClock(busClockAPB1);
    mant = apb1 * 16 / (sampling * baudSpeed);    // v 16-tinach
    tmp = mant / 16;

    frac = mant - (tmp * 16);
     // zbytek po deleni 16
    USART2->BRR = (tmp << 4) | (frac & 0x0f);

    /*
    int over8 = 0;
    float usart_div = (float)SystemCoreClock / ( baudSpeed * 8 * (2 - over8) );
    uint32_t brr_int = ((uint32_t) (usart_div/1)); // get integer part from  usart_div;
    float usart_div_frac = usart_div - brr_int;
    float minDiff = 1;
    uint32_t min_idx = 0;
    for (uint32_t i = 1; i < 16; i++ )
    {
      float absDiff = fabs( usart_div_frac - ((float)i/16.0) );
      if( minDiff >  absDiff)
      {
        min_idx = i;
        minDiff = absDiff;
      }
    }
    uint32_t brr_frac = min_idx;
    uint32_t brr = ( brr_int << 4 ) | brr_frac;
    USART2->BRR = brr; // rychlost 38400 pri 16MHz - spocitano predem
    */
    USART2->CR1 |= USART_CR1_UE; // az na zaver povolen blok USARTu
}

int Usart2Send(char c)
{
  while(!(USART2->SR & USART_SR_TXE))
    ; // cekej dokud neni volny TDR

  USART2->DR = c; // zapis do TDR k odeslani
  return c;
}

void Usart2String(char *txt)
{
  while(*txt)
  {
    Usart2Send(*txt);
    txt++;
  }
}

/*
int Usart2SendStr(char c[])
{
  for (int i = 0; i < strlen(c); i++)
  {
    Usart2Send(c[i]);
  }
  return 0;
}
*/
int Usart2Recv(void)
{
  while(!(USART2->SR & USART_SR_RXNE)) // cekej dokud neprijde
    ;
  return USART2->DR; // vycti a vrat jako hodnotu
}

bool IsUsart2Recv(void) // priznak, ze je neco v bufferu
{
  return (USART2->SR & USART_SR_RXNE) != 0;
  // podminka vynuti true/false vysledek
}

void Usart2RecvLine(char *buf)
{
  char *bufPtr = buf;
  if (IsUsart2Recv())
  {
    do
    {
      *bufPtr = Usart2Recv(); //(char)USART2->DR; // proc nefunguje pouze vycteni registru??
    }
    while( !( (*bufPtr == 0x0A) | (*bufPtr++ == 0x0D)) ); // Waiting for LF Line Feed or CR Carriage Return
    *(--bufPtr) = '\0';
  }
}

// Nefunguje - nacte pouze prvni znak -> RXNE je nastaven po precteni kazdeho bytu??
void Usart2RecvStr(char *buf)
{
  char *bufPtr = buf;
  if (IsUsart2Recv())
  {
    do
    {
      *bufPtr = (char)USART2->DR;
    }
    while( (USART2->SR & USART_SR_RXNE) != 0); // Read until RxNotEmpty
    *(--bufPtr) = '\0';
  }
}
