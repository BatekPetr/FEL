/******************************************************
*   MCBSTM32C AUXILLIARY    OUTPUT:                   *
*               PE8         PUSH-PULL                 *                 
*******************************************************/

#include "stm32f10x.h"

#define DIRECT_REG_ACCESS

extern uint32_t SystemCoreClock;                 // Variable defined in the system_stm32f10x.c
  
volatile unsigned int timer_1ms = 0;             // Global variable - 1 ms time reference                    

#ifdef DIRECT_REG_ACCESS

  #define GPIOE_MASK_H        0xFFFFFFF0         // GPIO configuration register mask - bits FEDCBA98

  #define PIN_SPEED           0x03               // GPIO output pin edge speed - 50 MHz

  #define AIRCR_VECTKEY       0x05FA0000         // Register SCB->AIRCR write key
  #define TIM6_IRQ_PRIORITY   0                  // Timer6 IRQ priority,0-highest priority, 15-lowest priority 
  #define GROUP4SUB0_PRIORITY 0x00000300         // Interrupt priority grouping field

  int main(void)
  {

      unsigned int tmp;

      /* MCBSTM32C auxilliary output PE8 initialization */

      RCC->APB2ENR  |= RCC_APB2ENR_IOPEEN ;       // Enable clock for GPIOE peripheral
      RCC->APB2RSTR |= RCC_APB2RSTR_IOPERST;      // Reset GPIOE peripheral
      RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPERST;     // Release GPIOE peripheral reset

      GPIOE->CRH &= GPIOE_MASK_H;                 // Configure PE8 pin 8 as output push-pull
      GPIOE->CRH |=(unsigned int)PIN_SPEED;

      /* Timer 6 initialization - 1ms time base*/
      
      RCC->APB1ENR  |=  RCC_APB1Periph_TIM6;     // Enable clock for TIM6 peripheral
      RCC->APB1RSTR |=  RCC_APB1Periph_TIM6;     // Reset TIM6 peripheral
      RCC->APB1RSTR &= ~RCC_APB1Periph_TIM6;     // Release TIM6 peripheral reset
      
      TIM6->CR1 = 0x0000;                        // ARPE = 0 (preload disable), URS = 0 (any event generates update)
                                                 // UDIS = 0 (update event enabled), CEN = 0  
      TIM6->PSC = SystemCoreClock/1000000 - 1;   // Prescale to 1 us timer clock using SystemCoreClock variable (72 MHz)                            
      TIM6->ARR = 1000;                          // Autoreload overflow value settings N * 1us (1ms) 
      TIM6->EGR = 0x0001;                        // Reinitialize and update timer, prescaler and reload registers
      TIM6->DIER |= TIM_DIER_UIE;                // Enable TIM6 update event interrupt - interrupt mode
      TIM6->DIER &= ~TIM_DIER_UDE;               // Disable DMA request
      TIM6->CR1 |= TIM_CR1_CEN;                  // Enable timer 

      /* Interrupt configuration - privileged Core-M3 mode is supposed. Otherwise switch into the privileged mode!!!*/ 

      tmp = SCB->AIRCR;                                         // Set priority grouping
      tmp &= ~(SCB_AIRCR_VECTKEY_Msk | SCB_AIRCR_PRIGROUP_Msk); // NVIC_PriorityGroup_x(misc.h), x = 0-4 
      tmp |= AIRCR_VECTKEY | GROUP4SUB0_PRIORITY;               // 4 bits for preemptive priority, 0 bits for subpriority 
      SCB->AIRCR = tmp;

      NVIC->IP[TIM6_IRQn] = ((uint8_t)TIM6_IRQ_PRIORITY) << 4;          // Set Timer6 IRQ priority
      NVIC->ISER[(uint32_t)TIM6_IRQn >> 5] = 1 << ((uint32_t)TIM6_IRQn & 0x1F); // Enable Timer6 IRQ   
      
      __enable_fault_irq();                      // Clear FAULTMASK flag - all exceptions enabled        
      __enable_irq();                            // Clear PRIMASK flags - all priority configurable exceptions enabled                          
      
      
      while(1)
      {  
	    
      }
     
  }

#else

  #define TIM6_IRQ_PREEMPT_PRIORITY   0            // Group priority 
  #define TIM6_IRQ_SUB_PRIORITY       0            // Subpriority


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
    TIM_TimeBaseInitStruct.TIM_Period = 1000;               // Autoreload overflow value settings N * 1us (1ms);
      
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStruct);        // Initialize TIM6 according to TIM_TimeBaseInitStruct
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);              // Enable update event interrupt - interrupt mode
    TIM_DMACmd(TIM6, TIM_DMA_Update, DISABLE);              // Disable DMA interrupt - polling mode
    TIM_Cmd(TIM6, ENABLE);                                  // Enable TIM6


    /* Interrupt configuration - privileged Core-M3 mode is supposed. Otherwise switch into the privileged mode!!!*/ 
                                                          // Set priority grouping - NVIC_PriorityGroup_x(misc.h), x = 0-4
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4 >> 8);  // 4 bits for preemptive priority, 0 bits for subpriority
    NVIC_SetPriority(TIM6_IRQn,\
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(),\
                     TIM6_IRQ_PREEMPT_PRIORITY,\
                     TIM6_IRQ_SUB_PRIORITY));             // Set Timer6 IRQ priority
    NVIC_EnableIRQ(TIM6_IRQn);                            // Enable Timer6 IRQ   
      
    __enable_fault_irq();                      // Clear FAULTMASK flag - all exceptions enabled        
    __enable_irq();                            // Clear PRIMASK flags - all priority configurable exceptions enabled                          


    while(1)
    {

    }
  }

#endif /* DIRECT_REG_ACCESS */

