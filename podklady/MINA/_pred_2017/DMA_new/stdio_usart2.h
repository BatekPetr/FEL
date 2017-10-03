#include <stdio.h>


  int fputc(int ch, FILE *f)
  {
     while(!(USART2->SR & USART_SR_TXE))    // Wait for transmit buffer empty
     {
     }       
     USART2->DR = ch; 

    return ch;
  }



  int fgetc(FILE *f)
  {
   while(!(USART2->SR & USART_SR_RXNE))  // Wait for receive buffer not empty
   {
   }       

   return USART2->DR;                    // Return received character
  }
