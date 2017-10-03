/*
 * nucleo_usart.c
 *
 *  Created on: Oct 2, 2017
 *      Author: batekp
 */

#include <nucleo_usart.h>

// USART Functions
void Usart2Init(int baudSpeed)
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

    //TODO doplnit vypocet BRR podle pozadovaneho a podle clocku
    USART2->BRR = 0x1A1; // rychlost 38400 pri 16MHz - spocitano predem
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
