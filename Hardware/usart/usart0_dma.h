#ifndef __USART0__DMA_H
#define __USART0__DMA_H

#include "gd32f4xx.h"

void usart_dma_init();

void usart_send_dma_data(uint8_t* data,int len);





#endif