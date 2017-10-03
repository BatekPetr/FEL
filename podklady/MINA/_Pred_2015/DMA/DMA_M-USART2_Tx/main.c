/***************************************************************
*   MCBSTM32C USART2 SIGNALS:                                  *
*                                                              *
*   RxD        PD6        Input floating (connect 2-3 on JP16) *
*   TxD        PD5        Alternate push-pull output           *
*                                                              *
****************************************************************/

#include "stm32f10x.h"

#define DIRECT_REG_ACCESS


const unsigned char SRC_Buffer[] = "\r\nHello DMA1 Channel7 !!! Thanks for your help. USART2 Transmitter\r\n";

#ifdef DIRECT_REG_ACCESS

  #define GPIOD_MASK_L        0xF00FFFFF         // GPIO configuration register mask - bits 76543210          


  int main(void)
  {    

				unsigned int keycode = 0;

  
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
      USART2->CR3 = USART_CR3_DMAT;              // Enable DMA transmitter, disable error interrupts, DMA receiver, HW flow control and other special modes
      USART2->BRR = 0x03AA;                      // Set bit rate to 38 400 bit/s at PCLK1 = 36MHz
      
     
	  /* DMA1 Channel7 initialization */

	  RCC->AHBENR |= RCC_AHBENR_DMA1EN;							// Enable DMA1 clock

	  DMA1_Channel7->CCR &= ~ DMA_CCR7_EN;						// Disable DMA1 Channel 7 to initialize
													
	  DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7			// Clear DMA1 Channel 7 status flags in DMA1->ISR
	                 | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);
	  DMA1_Channel7->CMAR  = (unsigned int)(&SRC_Buffer[0]);	// Initialize DMA memory source base address
	  DMA1_Channel7->CPAR  = (unsigned int)(&(USART2->DR));	    // Initialize DMA peripheral destination base address 
	  DMA1_Channel7->CNDTR = sizeof SRC_Buffer;					// Initialize number of transfers
														        // Configure DMA channel behavior
	  DMA1_Channel7->CCR =  DMA_CCR7_PL_0 | DMA_CCR7_PSIZE_1	// Priority Medium, Memsize 8 bits, Periphsize 32 bits
	                      | DMA_CCR7_MINC | DMA_CCR1_DIR;		// Memory incremented, Memory to peripheral transfer direction 
					   											// Peripheral not incremented
																// Normal mode CIRC = 0
																// Peripheral -> Memory mode DIR = 0,
																// Interrupts disabled: TEIE=HTIE=TCIE= 0

		       
		      while(keycode != 'D')
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
			  /*
		       while(!(USART2->SR & USART_SR_TXE))       // Wait for transmit buffer empty
		       {
		       }       
		       USART2->DR = 0x31;                     // Echo received character back to the terminal
			  */
		

	
	  DMA1_Channel7->CCR |= DMA_CCR7_EN;						// Enable DMA1 Channel 7 - Start DMA transfer

      while(!(DMA1->ISR & DMA_ISR_TCIF7))						// Wait for end of complete DMA transfer
	  {
	  // Test error flag DMA_ISR_TEIF7 in a real application!
	  }
	  DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7			// Clear DMA1 Channel 7 status flags in DMA1->ISR
	                 | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);	 

      while (1)													// Endless stop loop 
      {    
       
      }
  }   

#else

  int main(void)
  {
      GPIO_InitTypeDef GPIO_InitStruct;
      USART_InitTypeDef USART_InitStruct;
	  DMA_InitTypeDef  DMA_InitStruct;
  
      			unsigned int keycode; 
 
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
      USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);		  // Enable USART2 DMA Tx request
      USART_Cmd(USART2, ENABLE);                              // Enable USART2 peripheral in CR1 register


	  /* DMA1 Channel7 initialization */

	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	// Enable DMA1 clock
	  DMA_DeInit(DMA1_Channel7);							// Disable channel 7 to initialize, Initialize DMA registers into default states
  														
	 													// Initialize DMA init structure
  	  DMA_InitStruct.DMA_PeripheralBaseAddr = (unsigned int)(&(USART2->DR)); // DMA peripheral destination base address
 	  DMA_InitStruct.DMA_MemoryBaseAddr = (unsigned int)(&SRC_Buffer[0]);// DMA memory source base address
  	  DMA_InitStruct.DMA_BufferSize = sizeof SRC_Buffer;				 // Number of transfers
  	  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;					 // DMA transfer direction - memory->peripheral
 	  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		 // Peripheral increment mode disabled
  	  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;				 // Memory increment mode
  	  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // Peripheral element size 32 bits
  	  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		 // Memory element size 8 bits
  	  DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;						 // Normal mode CIRC = 0						
  	  DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;				 // Medium DMA priority
  	  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;							 // Memory to memory mode disabled
  	
	  DMA_Init(DMA1_Channel7, &DMA_InitStruct);			// Initialize DMA1 Channel7

	
		      while(keycode != 'D')
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
	 

	  DMA_Cmd(DMA1_Channel7, ENABLE);					// Enable DMA1 Channel 7 - Start DMA transfer

      while(DMA_GetFlagStatus(DMA1_FLAG_TC7)== RESET)	// Wait for end of complete DMA transfer
	  {
	    // Test error flag DMA_ISR_TEIF7 in a real application!
	  }
														// Clear DMA1 Channel 7 status flags in DMA1->ISR
	  DMA_ClearFlag(DMA1_FLAG_TC7 | DMA1_FLAG_HT7 | DMA1_FLAG_TE7 | DMA1_FLAG_GL7);

      while(1)
      {
      }
  }


#endif /* DIRECT_REG_ACCESS */

