/******************************************************
*   MCBSTM32C AUXILLIARY    OUTPUT:                   *
*               PE8         PUSH-PULL                 *                 
*******************************************************/

#include "stm32f10x.h"

#define DIRECT_REG_ACCESS

extern uint32_t SystemCoreClock;               // Variable defined in the system_stm32f10x.c

#ifdef DIRECT_REG_ACCESS

  #define GPIOE_MASK_H        0xFFFFFFF0         // GPIO configuration register mask - bits FEDCBA98

  #define PIN_SPEED           0x03               // GPIO output pin edge speed - 50 MHz


  int main(void)
  {

      /* MCBSTM32C auxilliary output PE8 initialization */

      RCC->APB2ENR  |= RCC_APB2ENR_IOPEEN ;       // Enable clock for GPIOE peripheral
      RCC->APB2RSTR |= RCC_APB2RSTR_IOPERST;      // Reset GPIOE peripheral
      RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPERST;     // Release GPIOE peripheral reset

      GPIOE->CRH &= GPIOE_MASK_H;				  // Configure PE8 pin 8 as output push-pull
      GPIOE->CRH |=(unsigned int)PIN_SPEED;

      /* Timer 6 initialization - 1ms time base*/
      
      RCC->APB1ENR  |=  RCC_APB1Periph_TIM6;     // Enable clock for TIM6 peripheral
      RCC->APB1RSTR |=  RCC_APB1Periph_TIM6;     // Reset TIM6 peripheral
      RCC->APB1RSTR &= ~RCC_APB1Periph_TIM6;     // Release TIM6 peripheral reset
      
      TIM6->CR1 = 0x0000;                        // ARPE = 0 (preload disable), URS = 0 (any event generates update)
                                                 // UDIS = 0 (update event enabled), CEN = 0  
      TIM6->PSC = SystemCoreClock/1000000 - 1;   // Prescale to 1 us timer clock using SystemCoreClock variable (72 MHz)                            
      TIM6->ARR = 999;                           // Autoreload overflow value settings (N+1) * 1us [1ms] 
      TIM6->EGR = 0x0001;                        // Reinitialize and update timer, prescaler and reload registers
      TIM6->DIER &= ~(TIM_DIER_UDE|TIM_DIER_UIE);// Disable interrupts and DMA request - polling mode example
      TIM6->CR1 |= TIM_CR1_CEN;                  // Enable timer 
      
      while(1)
      {    
         while(!(TIM6->SR&TIM_SR_UIF))           // Wait for overflow update
         {
         }
         TIM6->SR &= ~TIM_SR_UIF;                // Clear update event flag 
         GPIOE->ODR ^= GPIO_ODR_ODR8;            // Time period 1ms entry point  
      }
     
  }

#else


  int main(void)
  {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;         

    /* MCBSTM32C selected outputs initialization */
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);   // Enable clock for GPIOD peripheral
    GPIO_DeInit(GPIOE);                                     // Assert and immediately release GPIOD peripheral reset

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;               // Set output GPIOE Init Structure
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        // Set Output push-pull mode
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
    GPIO_Init(GPIOE, &GPIO_InitStructure);                  // Initialize output GPIOE pin 8

    /* Timer 6 initialization - 1ms time base*/
   
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);    // Enable clock for TIM6 peripheral
    TIM_DeInit(TIM6);                                       // Assert and immediately release TIM6 peripheral reset
  
    TIM_TimeBaseInitStruct.TIM_Prescaler = SystemCoreClock/1000000 - 1; // Prescale to 1 us timer clock using SystemCoreClock variable (24 Mhz)
    TIM_TimeBaseInitStruct.TIM_Period = 999;                // Autoreload overflow value settings N+1) * 1us [1ms];
      
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);        // Initialize TIM6 according to TIM_TimeBaseInitStruct
    TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);             // Disable update event interrupt - polling mode
    TIM_DMACmd(TIM6, TIM_DMA_Update, DISABLE);              // Disable DMA interrupt - polling mode
    TIM_Cmd(TIM6, ENABLE);                                  // Enable TIM6

    while(1)
    {
       while(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update)!= SET)// Wait for overflow update
       {
       }
       TIM_ClearFlag(TIM6, TIM_FLAG_Update);                // Clear update event flag 

       if(GPIO_ReadOutputDataBit(GPIOE,GPIO_Pin_8)== Bit_SET)    
           GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_RESET);
       else
           GPIO_WriteBit(GPIOE, GPIO_Pin_8, Bit_SET);
    }
  }

#endif /* DIRECT_REG_ACCESS */

