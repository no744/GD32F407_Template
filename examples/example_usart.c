#include "gd32f4xx.h"
#include <stdio.h>
#include "usart0.h"
#include <string.h>
#include "systick.h"
#if 1


void usart0_recv_callback(uint8_t* data,int len){
  printf("usart0_recv_callback:%s\r\n",data);

}
// ���ڲ��Դ��ڵķ��ͺͽ���
void example12_usart_test(){
  usart_init();
  printf("example12_usart_test\r\n");
  
}


#endif