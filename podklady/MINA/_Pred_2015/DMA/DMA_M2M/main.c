#include "stm32f10x.h"

#define DIRECT_REG_ACCESS

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


#ifdef DIRECT_REG_ACCESS

  int main(void)
  {

	RCC->AHBENR |= RCC_AHBENR_DMA1EN;							// Enable DMA1 clock

	DMA1_Channel7->CCR &= ~ DMA_CCR7_EN;						// Disable DMA1 Channel 7 to initialize
													
	DMA1->IFCR |= (DMA_IFCR_CTEIF7 | DMA_IFCR_CHTIF7			// Clear DMA1 Channel 7 status flags in DMA1->ISR
	               | DMA_IFCR_CTCIF7 | DMA_IFCR_CGIF7);
	DMA1_Channel7->CPAR	= (unsigned int)(&SRC_Buffer);			// Initialize DMA source base address
	DMA1_Channel7->CMAR	= (unsigned int)(&DST_Buffer);			// Initialize DMA destination base address
	DMA1_Channel7->CNDTR = BUFFSIZE;							// Initialize number of transfers
														// Configure DMA channel behavior
	DMA1_Channel7->CCR = DMA_CCR7_MEM2MEM | DMA_CCR7_PL_0		// Memory to memory, Priority Medium
	                   | DMA_CCR7_MSIZE_1 | DMA_CCR7_PSIZE_1	// Memsize 32 bits, Periphsize 32 bits
					   | DMA_CCR7_MINC | DMA_CCR7_PINC;			// Memory increment, Peripheral increment
					   											// Normal mode CIRC = 0
																// Peripheral -> Memory mode DIR = 0,
																// Interrupts disabled: TEIE=HTIE=TCIE= 0
	
	DMA1_Channel7->CCR |= DMA_CCR7_EN;							// Enable DMA1 Channel 7 - Start DMA transfer

    while(!(DMA1->ISR & DMA_ISR_TCIF7))							// Wait for end of complete DMA transfer
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

	DMA_InitTypeDef  DMA_InitStruct;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	// Enable DMA1 clock
	DMA_DeInit(DMA1_Channel7);							// Disable channel 7 to initialize, Initialize DMA registers into default states
  														
														// Initialize DMA init structure
  	DMA_InitStruct.DMA_PeripheralBaseAddr = (unsigned int)(&SRC_Buffer); // DMA source base address;
 	DMA_InitStruct.DMA_MemoryBaseAddr = (unsigned int)(&DST_Buffer);	 // DMA destination base address;
  	DMA_InitStruct.DMA_BufferSize = BUFFSIZE;							 // Number of transfers
  	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;						 // DMA transfer direction - data source
 	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;		 // Peripheral increment mode
  	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;				 // Memory increment mode
  	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // Peripheral element size 32 bits
  	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;		 // Memory element size 32 bits
  	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;							 // Normal mode CIRC = 0						
  	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;					 // Medium DMA priority
  	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;							 // Memory to memory mode
  	
	DMA_Init(DMA1_Channel7, &DMA_InitStruct);			// Initialize DMA1 Channel7

	DMA_Cmd(DMA1_Channel7, ENABLE);					    // Enable DMA1 Channel 7 - Start DMA transfer

    while(DMA_GetFlagStatus(DMA1_FLAG_TC7)== RESET)	    // Wait for end of complete DMA transfer
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

