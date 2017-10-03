/******************************************************
*   MCBSTM32C SELECTED OUTPUTS:                       *
*               PE8         PUSH-PULL                 *                 
*               PE9         PUSH-PULL                 *
*               PE10        PUSH-PULL                 *
*                                                     *
*   MCBSTM32C SELECTED INPUTS:                        *
*   USER        PB7         STANDBY HIGH/ACTIVE LOW   *
*   TAMPER      PC13        STANDBY HIGH/ACTIVE LOW   *
*   WAKEUP      PA0         STANDBY LOW/ACTIVE HIGH   *
*******************************************************/

#include "stm32f10x.h"

#define DIRECT_REG_ACCESS

#ifdef DIRECT_REG_ACCESS

  #define GPIOA_MASK_L        0xFFFFFFF0         // GPIO configuration register mask - bits 76543210          
  #define GPIOB_MASK_L        0x0FFFFFFF         // GPIO configuration register mask - bits 76543210
  #define GPIOC_MASK_H        0xFF0FFFFF         // GPIO configuration register mask - bits FEDCBA98
  #define GPIOE_MASK_H        0xFFFFF000         // GPIO configuration register mask - bits FEDCBA98

  #define PIN_SPEED           0x03               // GPIO output pin edge speed - 50 MHz
  #define IN_FLOATING         0x04				 // GPIO floating input pin configuration
  #define COUNT               10


  int main(void)
  {
      int i;

      /* MCBSTM32C selected outputs initialization */

      RCC->APB2ENR  |= RCC_APB2ENR_IOPEEN ;       // Enable clock for GPIOE peripheral
      RCC->APB2RSTR |= RCC_APB2RSTR_IOPERST;      // Reset GPIOE peripheral
      RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPERST;     // Release GPIOE peripheral reset

      GPIOE->CRH &= GPIOE_MASK_H;				  // Configure PD pins 8,9,10 as output push-pull
      GPIOE->CRH |= ((unsigned int)(PIN_SPEED<<(10-8)*4)|\
	                 (unsigned int)(PIN_SPEED<<(9-8)*4) |\
					 (unsigned int)(PIN_SPEED<<(8-8)*4));

      /* MCBSTM32C selected inputs initialization */

      RCC->APB2ENR  |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN);       // Enable clock for GPIOA, GPIOB and GPIOC peripherals
      RCC->APB2RSTR |= (RCC_APB2RSTR_IOPARST | RCC_APB2RSTR_IOPBRST | RCC_APB2RSTR_IOPCRST); // Reset GPIOA, GPIOB and GPIOC peripherals
      RCC->APB2RSTR &= ~(RCC_APB2RSTR_IOPARST | RCC_APB2RSTR_IOPBRST | RCC_APB2RSTR_IOPCRST);// Release GPIOA, GPIOB and GPIOC peripherals reset

      GPIOA->CRL &= GPIOA_MASK_L;				   // Configure PA0, PB7 and PC13 pins as input floating
      GPIOA->CRL |= IN_FLOATING;
      GPIOB->CRL &= GPIOB_MASK_L;
      GPIOB->CRL |=(unsigned int)(IN_FLOATING<<7*4);
      GPIOC->CRH &= GPIOC_MASK_H;
      GPIOC->CRH |= (unsigned int)(IN_FLOATING<<(13-8)*4);


      while (1)
      {    
       
          if(GPIOB->IDR & GPIO_IDR_IDR7)
          {
              GPIOE->ODR ^= GPIO_ODR_ODR8;                
              for(i=0; i< COUNT; i++) 
              {
              }
          }
          else
          {
              if(GPIOC->IDR & GPIO_IDR_IDR13)
              {         
                  GPIOE->ODR |= GPIO_ODR_ODR9;
                  GPIOE->ODR &= ~GPIO_ODR_ODR10;
              }
              else
              {    
                  GPIOE->ODR &= ~GPIO_ODR_ODR9;
                  GPIOE->ODR |= GPIO_ODR_ODR10;
              }          
          }           
      }
      
  }

#else

  #define COUNT       10

  int main(void)
  {
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;

    /* MCBSTM32C selected outputs initialization */
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);   // Enable clock for GPIOD peripheral
    GPIO_DeInit(GPIOE);                                     // Assert and immediately release GPIOD peripheral reset

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 ; // Set output GPIOE Init Structure
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                      // Set Output push-pull mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
    GPIO_Init(GPIOE, &GPIO_InitStructure);                  // Initialize output GPIOE pins 8, 9, 10

  
    /* MCBSTM32C selected inputs initialization */
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE); // Enable clock for GPIOA, GPIOB, GPIOC peripherals
    GPIO_DeInit(GPIOA);                                     // Assert and immediately release GPIOA peripheral reset
    GPIO_DeInit(GPIOB);                                     // Assert and immediately release GPIOB peripheral reset
    GPIO_DeInit(GPIOC);                                     // Assert and immediately release GPIOC peripheral reset


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;              // Set input GPIOA Init Structure
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // Set Input floating
    GPIO_Init(GPIOA, &GPIO_InitStructure);                  // Initialize input GPIOA pin 0

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;              // Modify input GPIOB Init Structure
    GPIO_Init(GPIOB, &GPIO_InitStructure);                  // Initialize input GPIOB pin 9
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;             // Modify input GPIOC Init Structure
    GPIO_Init(GPIOC, &GPIO_InitStructure);                  // Initialize input GPIOC pin 13


    while(1)
    {
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7))
        {    
            if(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_8)== Bit_SET)    
                GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_RESET);
            else
                GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_SET);
       
            for(i=0; i< COUNT; i++) 
            {
            }
        }
        else
        {
            if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))
            {
                GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET);
                GPIO_WriteBit(GPIOE, GPIO_Pin_10, Bit_RESET);
            }
            else
            {
                GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET);
                GPIO_WriteBit(GPIOE, GPIO_Pin_10, Bit_SET);
            }
        }
    }
  }

#endif /* DIRECT_REG_ACCESS */

