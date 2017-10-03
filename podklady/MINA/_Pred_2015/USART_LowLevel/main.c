/***************************************************************
*   MCBSTM32C USART2 SIGNALS:                                *
*                                                              *
*   RxD        PD6        Input floating (connect 2-3 on JP16) *
*   TxD        PD5        Alternate push-pull output           *
*                                                              *
****************************************************************/

#include "stm32f10x.h"

#define	BIT_RATE_38400		0x03AA
#define	BIT_RATE_57600		0x0271
#define	BIT_RATE_115200		0x0138


#define DIRECT_REG_ACCESS


#ifdef DIRECT_REG_ACCESS

  #define GPIOD_MASK_L        0xF00FFFFF         // GPIO configuration register mask - bits 76543210          


  int main(void)
  {    
  
      unsigned int keycode; 
  
      /* USART2 pin initialization (PD5 into alternate push-pull and PD6 into input floating) */

      RCC->APB2ENR  |=  (RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN);     // Enable clocks for GPIOD and AFIO peripherals
      RCC->APB2RSTR |=  (RCC_APB2RSTR_IOPDRST | RCC_APB2RSTR_AFIORST); // Reset GPIOD and AFIO peripherals
      RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPDRST | RCC_APB2RSTR_AFIORST); // Release GPIOD and AFIO peripheral reset

      AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;     // Remap USART2 signals RxD and TxD to alternate pins PD6 and PD5
      
      GPIOD->CRL &= GPIOD_MASK_L;
      GPIOD->CRL |= ((unsigned int)((((GPIO_Mode_AF_PP & 0x0F)|GPIO_Speed_50MHz)<<4*5)| \
                     (GPIO_Mode_IN_FLOATING << 4*6)));                 // Configure USART pins

      /* USART2 peripheral initialization */
  
      RCC->APB1ENR  |=  RCC_APB1ENR_USART2EN;    // Enable clocks for USART2
      RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;  // Reset USART2 peripheral
      RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;  // Release USART2 peripheral reset
  
      USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; // Global UART enable, 8-bit transfer, Tx enable, Rx enable
      USART2->CR2 = 0x0000;                      // Set 1 stop bit
      USART2->CR3 = 0x0000;                      // Disable error interrupts, DMA, HW flow control and other special modes
      USART2->BRR = BIT_RATE_38400;              // Set bit rate at PCLK1 = 36MHz
      
      while(1)
      {
       while(!(USART2->SR & USART_SR_RXNE))      // Wait for receive buffer not empty
       {
       }       
       keycode = USART2->DR;                     // Read received keycode
      
       while(!(USART2->SR & USART_SR_TXE))       // Wait for transmit buffer empty
       {
       }       
       USART2->DR = keycode;                     // Echo received character back to the terminal
      }
   }   

#else

  int main(void)
  {
      GPIO_InitTypeDef GPIO_InitStruct;
      USART_InitTypeDef USART_InitStruct;
  
      unsigned short int keycode; 
 
      /* USART2 pins initialization (PD5 into alternate push-pull and PD6 into input floating) */
  
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);  // Enable clock for GPIOD and AFIO peripherals
      GPIO_DeInit(GPIOD);                                     // Assert and immediately release GPIOD peripheral reset
      GPIO_AFIODeInit();                                      // Assert and immediately release AFIO peripheral reset

      GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);         // Remap USART2 to alternate pins PD5 and PD6
  
      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;                  // Set PD5 pin structure - Tx 
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;            // Set alternate push-pull mode
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
      GPIO_Init(GPIOD, &GPIO_InitStruct);                     // Initialize PD5 pin - USART2 Tx

      GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 ;                 // Set PD6 pin structure - Rx 
      GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;      // Set Input floating mode
  
      GPIO_Init(GPIOD, &GPIO_InitStruct);                     // Initialize PD6 pin - USART2 Rx

       /* USART2 peripheral initialization */
      
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  // Enable clock for USART2 peripheral
      USART_DeInit(USART2);                                   // Assert and immediately release USART2 peripheral reset
      
      USART_InitStruct.USART_BaudRate = 38400;                // Initialize USART_InitStruct by required values 
      USART_InitStruct.USART_WordLength = USART_WordLength_8b;
      USART_InitStruct.USART_StopBits = USART_StopBits_1;
      USART_InitStruct.USART_Parity = USART_Parity_No;
      USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
      USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

      USART_Init(USART2,&USART_InitStruct);                   // Initialize USART2 peripheral registers
      USART_Cmd(USART2, ENABLE);                              // Enable USART2 peripheral in CR1 register
      

      while(1)
      {    
         while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
         {
         }
         keycode = USART_ReceiveData(USART2);

         USART_SendData(USART2, keycode);
         while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
         {
         }
      }

  }


#endif /* DIRECT_REG_ACCESS */

