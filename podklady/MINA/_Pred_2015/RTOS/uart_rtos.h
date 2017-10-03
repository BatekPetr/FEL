#ifndef __UART_RTOS_H
#define __UART_RTOS_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifndef bool
typedef int bool;
#define false (0)
#define true (!false)
#endif

#ifndef byte
typedef unsigned char byte;
#endif

#define mainUSART_TASK_PRIORITY         ( tskIDLE_PRIORITY + 1 )
#define mainUSART_TASK_STACK_SIZE       configMINIMAL_STACK_SIZE + 50

#define USING_UART2_INT

extern portBASE_TYPE xStringToUartQueue(char *src);
extern bool InitUART2RTOS(void);

//extern void UARTTask(void *params);

#endif
