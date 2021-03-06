/*
 * nucleo_usart.h
 *
 *  Created on: Oct 2, 2017
 *      Author: batekp
 */

#ifndef NUCLEO_USART_H_
#define NUCLEO_USART_H_

#include "nucleo_core.h"
#include <stdio.h>

void Usart2Init(int baudSpeed);

int Usart2Send(char c);
int Usart2Recv(void);
bool IsUsart2Recv(void);
void Usart2String(char *txt);
int Usart2SendStr(char c[]);

#endif /* NUCLEO_USART_H_ */
