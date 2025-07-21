#ifndef __USART0__H
#define __USART0__H

#include "gd32f4xx.h"

void usart_init();

void usart0_send_data(uint8_t* data,int len);





#endif