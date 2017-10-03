#include "stm32f10x.h"
#include "stdio_usart2.h"

#define	BIT_RATE_38400		      0x03AA
#define	BIT_RATE_57600		      0x0271
#define	BIT_RATE_115200		      0x0138

#define IN_FLOATING               0x04		 // GPIO floating input pin configuration
#define AF_PP         		        0x08		 // GPIO alternative output function - push-pull  pin configuration
#define PIN_SPEED                 0x03     // GPIO output pin edge speed - 50 MHz


//#define MEM_TO_MEM_COPY
//#define SIMULATE_HARD_FAULT

#define MEM_TO_MEM_DMA

//#define UART_Tx_DMA
//#define UART_Tx_DMA_CIRCULAR

volatile unsigned int timer_1us;



#ifdef UART_Tx_DMA

    const unsigned char SRC_Buffer[] = "\r\nHello DMA1 Channel7 !!! Thanks for your help. USART2 Transmitter\r\n";


    int main(void)
    { 
      unsigned int keycode = 0;
      
    /* USART2 Initialization */  
      
      if(!(RCC->APB1ENR & RCC_APB1ENR_USART2EN))
      {
        RCC->APB1ENR  |=  RCC_APB1ENR_USART2EN;   // Enable clocks for USART2
        RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST; // Reset USART2 peripheral
        __nop();
        RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST; // Release USART2 peripheral reset
      }

      USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; // Global UART enable, 8-bit transfer, Tx enable, Rx enable
      USART2->CR2 = 0x0000;                       // Set 1 stop bit
      USART2->CR3 = USART_CR3_DMAT;               // Enable DMA for USART transmitter
                                                  // Disable error interrupts, HW flow control and other special modes
      USART2->BRR = BIT_RATE_38400;               // Set bit rate at PCLK1 = 36MHz

      if(!(RCC->APB2ENR & RCC_APB2ENR_IOPAEN))
      {
        RCC->APB2ENR  |=  RCC_APB2ENR_IOPAEN;     // Enable clocks for GPIOA peripheral
        RCC->APB2RSTR |=  RCC_APB2RSTR_IOPARST;   // Reset GPIOA peripheral
        __nop();
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPARST); // Release GPIOA peripheral reset
      }

      GPIOA->CRL &= 0xFFFF00FF;
      GPIOA->CRL |= (unsigned int)(((AF_PP | PIN_SPEED)<<4*2)|(IN_FLOATING << 4*3)); // Configure USART pins
      
      puts("\r\nUART2 stdio driver loaded\r");

    /* DMA1 Channel7 initialization */

      RCC->AHBENR |= RCC_AHBENR_DMA1EN;						// Enable DMA1 clock

      DMA1_Channel7->CCR &= ~ DMA_CCR7_EN;				// Disable DMA1 Channel 7 to initialize
                            
      DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7		    // Clear DMA1 Channel 7 status flags in DMA1->ISR
                     | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);
      DMA1_Channel7->CMAR  = (unsigned int)(&SRC_Buffer[0]);	// Initialize DMA memory source base address
      DMA1_Channel7->CPAR  = (unsigned int)(&(USART2->DR));	  // Initialize DMA peripheral destination base address 
      DMA1_Channel7->CNDTR = sizeof SRC_Buffer;		// Initialize number of transfers
                                                  // Configure DMA channel behavior
      DMA1_Channel7->CCR =  DMA_CCR7_PL_0 | DMA_CCR7_PSIZE_1	// Priority Medium, Memsize 8 bits, Periphsize 32 bits
                          | DMA_CCR7_MINC | DMA_CCR1_DIR;		  // Memory incremented, Memory to peripheral transfer direction 
                                                              // Peripheral not incremented
                                                              // Normal mode CIRC = 0
                                                              // Peripheral -> Memory mode DIR = 0,
                                                              // Interrupts disabled: TEIE=HTIE=TCIE= 0
  #ifdef UART_Tx_DMA_CIRCULAR
  
      DMA1_Channel7->CCR |= DMA_CCR7_CIRC;         // Enable DMA circular mode

  #endif /* UART_Tx_DMA_CIRCULAR */
                                                              
      puts("\rDMA1 Channel7 initialized\r");
             
      while(keycode != 'D')
      {
        keycode = getchar();                 // Read received keycode
        putchar(keycode);                    // Echo received character back to the terminal
      }  
      
      
      DMA1_Channel7->CCR |= DMA_CCR7_EN;		// Enable DMA1 Channel 7 - Start DMA transfer

      while(!(DMA1->ISR & DMA_ISR_TCIF7))   // Wait for end of complete DMA transfer
      {
        // Test error flag DMA_ISR_TEIF7 in a real application!
      }
      DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7	      // Clear DMA1 Channel 7 status flags in DMA1->ISR
                     | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);	 

      while (1)													// Endless stop loop 
      {    
       
      }
    }
#endif /* UART_Tx_DMA */
    

#ifdef MEM_TO_MEM_DMA

  #define BUFFSIZE  32

  const unsigned int SRC_Buffer[BUFFSIZE]= {  0x03020100,0x13121110,0x23232120,0x33323130,
                                              0x43424140,0x53525150,0x63626160,0x73727170,
                                              0x83828180,0x93929190,0xA3A2A1A0,0xB3B2B1B0,
                                              0xC3C2C1C0,0xD3D2D1D0,0xE3E2E1E0,0xF3F2F1F0,
                                              0x07060504,0x17161514,0x27262524,0x37363534,
                                              0x47464544,0x57565554,0x67666564,0x77767574,
                                              0x87868584,0x97969594,0xA7A6A5A4,0xB7B6B5B4,
                                              0xC7C6C5C4,0xD7D6D5D4,0xE7E6E5E4,0xF7F6F5F4 };

  unsigned int DST_Buffer[BUFFSIZE];


  int main(void)
  {

    RCC->AHBENR |= RCC_AHBENR_DMA1EN;							// Enable DMA1 clock

    DMA1_Channel7->CCR &= ~ DMA_CCR7_EN;					// Disable DMA1 Channel 7 to initialize
                            
    DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7		// Clear DMA1 Channel 7 status flags in DMA1->ISR
                   | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);
    DMA1_Channel7->CPAR	= (unsigned int)(&SRC_Buffer);	// Initialize DMA source base address
    DMA1_Channel7->CMAR	= (unsigned int)(&DST_Buffer);	// Initialize DMA destination base address
    DMA1_Channel7->CNDTR = BUFFSIZE;							// Initialize number of transfers
                                                  // Configure DMA channel behavior
    DMA1_Channel7->CCR = DMA_CCR7_MEM2MEM | DMA_CCR7_PL_0		  // Memory to memory, Priority Medium
                        | DMA_CCR7_MSIZE_1 | DMA_CCR7_PSIZE_1	// Memsize 32 bits, Periphsize 32 bits
                        | DMA_CCR7_MINC | DMA_CCR7_PINC;			// Memory increment, Peripheral increment
                                                              // Normal mode CIRC = 0
                                                              // Peripheral -> Memory mode DIR = 0,
                                                              // Interrupts disabled: TEIE=HTIE=TCIE= 0
    
    DMA1_Channel7->CCR |= DMA_CCR7_EN;						// Enable DMA1 Channel 7 - Start DMA transfer

    while(!(DMA1->ISR & DMA_ISR_TCIF7))						// Wait for end of complete DMA transfer
    {
      // Test error flag DMA_ISR_TEIF7 in a real application!
    }
    DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7			    // Clear DMA1 Channel 7 status flags in DMA1->ISR
                  | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);	 

    while (1)													          // Endless stop loop 
    {    
       
    }
  }
    
#endif /* MEM_TO_MEM_DMA */ 


#ifdef MEM_TO_MEM_COPY
  
  #define BUFFSIZE  2048    /* 32 * 4B -> 128 B */
                            /* 2048 * 4B -> 8192B (8 kB */

  #ifndef SIMULATE_HARD_FAULT
    unsigned int Src_Buffer[BUFFSIZE];
    unsigned int Dst_Buffer[BUFFSIZE];
 
    void InitBuffers(void)
    {
      int i;
      
      for(i=0; i < BUFFSIZE; i++)
      {
        Src_Buffer[i] = i;
        Dst_Buffer[i] = 0U;
      }
    }
  #endif /*SIMULATE_HARD_FAULT*/

  
  
  int main(void) 
  {
    unsigned int i;
    unsigned int *ptrSrc, *ptrDst;
    unsigned int timestart, timestop;
  
    #ifdef SIMULATE_HARD_FAULT
      unsigned int Src_Buffer[BUFFSIZE];
      unsigned int Dst_Buffer[BUFFSIZE];
    #endif /*SIMULATE_HARD_FAULT*/
  
    /* SysTick initialization - privileged Core-M3 mode is supposed. Otherwise switch into the privileged mode!!!*/ 

    SysTick->LOAD = SystemCoreClock/1000000 - 1;  // Preload to 1us interval
    SysTick->VAL = 	0x00000000;
    SysTick->CTRL = (unsigned int)(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk); 

    /* USART2 Initialization */  
    
    if(!(RCC->APB1ENR & RCC_APB1ENR_USART2EN))
    {
      RCC->APB1ENR  |=  RCC_APB1ENR_USART2EN;   // Enable clocks for USART2
      RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST; // Reset USART2 peripheral
      __nop();
      RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST; // Release USART2 peripheral reset
    }

    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; // Global UART enable, 8-bit transfer, Tx enable, Rx enable
    USART2->CR2 = 0x0000;                       // Set 1 stop bit
    USART2->CR3 = USART_CR3_DMAT;               // Enable DMA for USART transmitter
                                                // Disable error interrupts, HW flow control and other special modes
    USART2->BRR = BIT_RATE_38400;               // Set bit rate at PCLK1 = 36MHz

    if(!(RCC->APB2ENR & RCC_APB2ENR_IOPAEN))
    {
      RCC->APB2ENR  |=  RCC_APB2ENR_IOPAEN;     // Enable clocks for GPIOA peripheral
      RCC->APB2RSTR |=  RCC_APB2RSTR_IOPARST;   // Reset GPIOA peripheral
      __nop();
      RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPARST); // Release GPIOA peripheral reset
    }

    GPIOA->CRL &= 0xFFFF00FF;
    GPIOA->CRL |= (unsigned int)(((AF_PP | PIN_SPEED)<<4*2)|(IN_FLOATING << 4*3)); // Configure USART pins
    
    printf("\r\nMemory copy routines - %d bytes transfer - relative speed test at %d MHz system clock:\r\n", BUFFSIZE, SystemCoreClock);
 
  #ifndef SIMULATE_HARD_FAULT
    InitBuffers();
  #endif 
    timestart = timer_1us; 
    
    for(i = 0; i < BUFFSIZE; i++)
    {
      Dst_Buffer[i] = Src_Buffer[i];
    }
    timestop = timer_1us; 
    
    printf("Time for copy of array: %d us\r\n",timestop - timestart);
  
  #ifndef SIMULATE_HARD_FAULT
    InitBuffers();
  #endif
    
    timestart = timer_1us; 
    
    ptrSrc = &Src_Buffer[0];
    ptrDst = &Dst_Buffer[0];
    
    for(i = 0; i < BUFFSIZE; i++)
    {
      *ptrDst++ = *ptrSrc++;
    }
    timestop = timer_1us; 
    
    printf("Time for pointer copy: %d us\r\n",timestop - timestart);
 
   #ifndef SIMULATE_HARD_FAULT
    InitBuffers();
   #endif 

    RCC->AHBENR |= RCC_AHBENR_DMA1EN;							// Enable DMA1 clock

    DMA1_Channel7->CCR &= ~ DMA_CCR7_EN;					// Disable DMA1 Channel 7 to initialize
                            
    DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7		// Clear DMA1 Channel 7 status flags in DMA1->ISR
                   | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);
    DMA1_Channel7->CPAR	= (unsigned int)(&Src_Buffer);	// Initialize DMA source base address
    DMA1_Channel7->CMAR	= (unsigned int)(&Dst_Buffer);	// Initialize DMA destination base address
    DMA1_Channel7->CNDTR = BUFFSIZE;							// Initialize number of transfers
                                                  // Configure DMA channel behavior
    DMA1_Channel7->CCR = DMA_CCR7_MEM2MEM | DMA_CCR7_PL_0		  // Memory to memory, Priority Medium
                        | DMA_CCR7_MSIZE_1 | DMA_CCR7_PSIZE_1	// Memsize 32 bits, Periphsize 32 bits
                        | DMA_CCR7_MINC | DMA_CCR7_PINC;			// Memory increment, Peripheral increment
                                                              // Normal mode CIRC = 0
                                                              // Peripheral -> Memory mode DIR = 0,
                                                              // Interrupts disabled: TEIE=HTIE=TCIE= 0
        
    timestart = timer_1us; 


    DMA1_Channel7->CCR |= DMA_CCR7_EN;						// Enable DMA1 Channel 7 - Start DMA transfer

    while(!(DMA1->ISR & DMA_ISR_TCIF7))						// Wait for end of complete DMA transfer
    {
      // Test error flag DMA_ISR_TEIF7 in a real application!
    }
    DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7			    // Clear DMA1 Channel 7 status flags in DMA1->ISR
                  | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);	 

    timestop = timer_1us; 
    
    printf("Time DMA transfer: %d us\r\n",timestop - timestart);

    while(1)
    {
    }
   
  }
 
#endif /* MEM_TO_MEM_COPY */  
    
