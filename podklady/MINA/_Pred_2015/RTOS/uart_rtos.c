/**
  ******************************************************************************
  * @file    uart_rtos.c
  * @author  PW
  * @version 1.00.00
  * @date    31.10.2014
  * @brief   UART2 for FreeRTOS support/functions

        RTOS example on BareBoard project
*/
#include <stm32f10x.h>

#include "FreeRTOS.h"

#include "uart_rtos.h"

#define USING_UART2_INT

/** USART2 pin initialization (PD5 into alternate push-pull and PD6 into input floating) **/
static bool SetupUART2(int baudrate)
{
  if (!(RCC->APB2ENR & RCC_APB2ENR_IOPDEN))
  {
		RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;       // Enable clocks for GPIOD peripheral
		RCC->APB2RSTR |= RCC_APB2RSTR_IOPDRST;    // Reset GPIOD peripheral
		RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPDRST;   // Release reset
  }

	GPIOD->CRL &= 0xf00fffff;                 // bity 76543210 - clear 6, 5
	GPIOD->CRL |= (unsigned int)0x0b << (5 * 4);  // bit 5 config: 1011 = AF push-pull, output mode 50MHz
	GPIOD->CRL |= (unsigned int)0x04 << (6 * 4);  // bit 6 config: 0100 = floating input, input mode

  if ((RCC->APB2ENR & RCC_APB2ENR_AFIOEN) == 0)
  {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2RSTR |= RCC_APB2RSTR_AFIORST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_AFIORST;
  }

	AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;     // Remap USART2 signals RxD and TxD to alternate pins PD6 and PD5

	/* USART2 peripheral initialization */
  if (!(RCC->APB1ENR & RCC_APB1ENR_USART2EN))
  {
    RCC->APB1ENR  |=  RCC_APB1ENR_USART2EN;    // Enable clocks for USART2
    RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;  // Reset USART2 peripheral
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;  // Release USART2 peripheral reset
  }
  
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE; // Global UART enable, 8-bit transfer, Tx enable, Rx enable
	USART2->CR2 = 0x0000;                      // Set 1 stop bit
	USART2->CR3 = 0x0000;                      // Disable error interrupts, DMA, HW flow control and other special modes
  
  switch(baudrate)
  {
    case 38400:
      USART2->BRR = 0x03AA;                      // Set bit rate to 38 400 bit/s at PCLK1 = 36MHz
      break;
    default:
      return false;
  }
  
#ifdef USING_UART2_INT
  USART2->CR1 |= USART_CR1_RXNEIE | USART_CR1_TCIE;  // enable generating INTs Recv Exmpty, Transmit Complete
  NVIC_EnableIRQ(USART2_IRQn);
#endif
  
  return true;
}

static xQueueHandle qRecv = NULL;
static xQueueHandle qSend = NULL;

#define BUF_SIZE_RECV 16
#define BUF_SIZE_SEND 32

/** 
    Inicializace UART2 na rychlost 38400 a vyuziti IRQ
    Konfigurace RTOS front
*/
bool InitUART2RTOS(void)
{
  bool bbResult = false;
  
  bbResult = SetupUART2(38400);
  if (bbResult)
  {
    qRecv = xQueueCreate(BUF_SIZE_SEND, sizeof(byte));
    qSend = xQueueCreate(BUF_SIZE_RECV, sizeof(byte));

    NVIC_SetPriority(USART2_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY + 1);
  }
  return bbResult;
}


void USART2_IRQHandler(void)
{
	if (USART2->SR & USART_SR_RXNE)  // neco prislo
	{
		byte b = USART2->DR;
		
		if (qRecv != NULL)
			xQueueSendFromISR(qRecv, &b, 0);
	}
	
	if (USART2->SR & USART_SR_TC)     // je  odeslano
	{
		byte b = 0;
		portBASE_TYPE x = pdFALSE;
		
		if ((qSend != NULL) &&
			(uxQueueMessagesWaitingFromISR(qSend) > 0) &&
		  (xQueueReceiveFromISR(qSend, &b, &x) != errQUEUE_EMPTY))
			USART2->DR = b;
		else
			USART2->CR1 &= ~USART_CR1_TCIE;
	}
}

portBASE_TYPE xStringToUartQueue(char *src)
{
  bool firstRun = true;
  
  if (src == NULL)
    return pdFAIL;
  
  if (qSend == NULL)
    return pdFAIL;
  
  while(*src)
  {
    if (xQueueSend(qSend, src, 10 / portTICK_PERIOD_MS) == errQUEUE_FULL)   // ceka 10ms na uspesnost
    {
//      LEDToggle(10);
      break;
    }
    
    if (firstRun)
    {
      firstRun = false;
      USART2->CR1 |= USART_CR1_TCIE;
    }      
    
    src++;
  }

  return *src ? errQUEUE_FULL : pdPASS;   // neni-li na konci, vraci chybu FULL
}

#define BUF_SIZE      8

/** Task, ktery prijima data z prichozi fronty, uklada do bufferu a po naplneni posle zpet jako "balik"

*/
void UARTTask(void *params)
{
	byte buf[BUF_SIZE];
	byte curPos = 0;
	
	portTickType last = xTaskGetTickCount();
	
	while(1)
	{
		if ((qRecv != NULL) && (qSend != NULL))
		{
			if (uxQueueMessagesWaiting(qRecv) > 0)
			{
				if (curPos < BUF_SIZE_RECV)
				{
					byte b = 0;
					xQueueReceive(qRecv, &b, 0);
					
//					LEDToggle(8);
					
					buf[curPos] = b;
					curPos++;
					
					if (curPos >= BUF_SIZE)
					{
						do
						{
							curPos--;
							xQueueSend(qSend, buf + curPos, 0);
						} while(curPos != 0);
						
//						LEDToggle(9);
						
						USART2->CR1 |= USART_CR1_TCIE;
					}
				}
			}
		}
		
		vTaskDelayUntil(&last, 10);
	}
}
