/***************************************************************
*   MCBSTM32C USART2 SIGNALS:                                *
*                                                              *
*   RxD        PD6        Input floating (connect 2-3 on JP16) *
*   TxD        PD5        Alternate push-pull output           *
*                                                              *
****************************************************************/

#include "stm32f10x.h"
#include "stdio_usart2.h"

#define IN_FLOATING         0x04		 // GPIO floating input pin configuration
#define AF_PP         		0x08		 // GPIO alternative output function - push-pull  pin configuration
#define PIN_SPEED           0x03         // GPIO output pin edge speed - 50 MHz

#define DIRECT_REG_ACCESS

#ifdef DIRECT_REG_ACCESS


  int main(void)
  {    
	
	  unsigned int keycode; 
	
	  RCC->APB2ENR  |=  (RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN);     // Enable clocks for GPIOD and AFIO peripherals
	  RCC->APB2RSTR |=  (RCC_APB2RSTR_IOPDRST | RCC_APB2RSTR_AFIORST); // Reset GPIOD and AFIO peripherals
		__nop();
	  RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPDRST | RCC_APB2RSTR_AFIORST); // Release GPIOD and AFIO peripheral reset
	
	  AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;     // Remap USART2 signals RxD and TxD to alternate pins PD6 and PD5
	  
	  GPIOD->CRL &= 0xF00FFFFF;
	  GPIOD->CRL |= (unsigned int)(((AF_PP | PIN_SPEED)<<4*5)|(IN_FLOATING << 4*6)); // Configure USART pins
	
	  RCC->APB1ENR  |=  RCC_APB1ENR_USART2EN;    // Enable clocks for USART2
	  RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;  // Reset USART2 peripheral
	  RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;  // Release USART2 peripheral reset
	
	  USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; // Global UART enable, 8-bit transfer, Tx enable, Rx enable
	  USART2->CR2 = 0x0000;                      // Set 1 stop bit
	  USART2->CR3 = 0x0000;                      // Disable error interrupts, DMA, HW flow control and other special modes
	  USART2->BRR = 0x03AA;                      // Set bit rate to 38 400 bit/s at PCLK1 = 36MHz
	  
	  puts("\r\nUART2 stdio driver loaded\r\n");
	
	  while(1)
	  {
		keycode = getchar();
		putchar(keycode);		
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

	  puts("\r\nUART2 stdio driver loaded\r\n");
	   
	  while(1)
	  {
		keycode = getchar();
		putchar(keycode);		
	  }

  }

#endif /* DIRECT_REG_ACCESS */
   
