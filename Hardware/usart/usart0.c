#include "usart0.h"
#include <stdio.h>
//串口usart封装
#if 1
uint8_t buffer[512];
int cnt=0;
void usart_init(){
   
	rcu_periph_clock_enable(RCU_GPIOA);
	 rcu_periph_clock_enable(RCU_USART0);
	
	 gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_9);
	 gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_10);
	
	 gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_9);
	 gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_10);
	
	 gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_MAX,GPIO_PIN_9);
	 gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_MAX,GPIO_PIN_10);
	
	 usart_deinit(USART0);
	 usart_baudrate_set(USART0,115200);
	 usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
	
	 usart_receive_config(USART0,USART_RECEIVE_ENABLE);
	 nvic_irq_enable(USART0_IRQn, 0, 0);
	
	
	 usart_interrupt_enable(USART0, USART_INT_RBNE);
	 usart_interrupt_enable(USART0, USART_INT_IDLE);
	 usart_enable(USART0);

}

void usart0_send_data(uint8_t* data,int len){
    
	  while(data && len){
		  usart_data_transmit(USART0,*data);
			while(RESET==usart_flag_get(USART0,USART_FLAG_TBE));
			data++;
			len--;
		}
	

}
void USART0_IRQHandler(void)
{
   if(SET==usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE)){
	     uint8_t data= usart_data_receive(USART0);
		   buffer[cnt]=data;
		   cnt++;
      //printf("data:%d \r\n");
		  
		  usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
		    
	 }
	 if(SET==usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE)){
		 buffer[cnt]='\0';
		 printf("idle cnt :%d %s\r\n",cnt,buffer);
     
//		 usart_interrupt_flag_clear(USART0,USART_INT_FLAG_IDLE);
	  
		 extern void usart0_recv_callback(uint8_t *data,int len);
		 usart0_recv_callback(buffer,cnt);
		 cnt=0;
		 usart_data_receive(USART0);
	 }
}
int fputc(int ch, FILE *f)  // 定义fputc,因为printf底层会调用这个函数
{   
    // 串口发送功能                0-255     
    usart_data_transmit(USART0,ch);
  // TBE : Transmit data Buffer Empty 发送数据的缓冲区为空
    // 判断是否发送完成, 等待这一个字节数据发送完成
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}
#endif