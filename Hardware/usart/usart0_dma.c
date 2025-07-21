#include "usart0_dma.h"
#include <stdio.h>
//串口usart封装
#if 1

#define RX_DMA_PORT DMA1
#define RX_DMA_CH   DMA_CH5
#define RX_DMA_PERIPH RX_DMA_PORT,RX_DMA_CH

#define DMA_TX_PORT    DMA1
#define DMA_TX_CH      DMA_CH7
#define DMA_TX_PERIPH  DMA_TX_PORT,DMA_TX_CH



#define RECV_BUFFER_LEN 512
uint8_t g_recv_buffer[RECV_BUFFER_LEN];
int recv_cnt = 0;



static void dma_rx_config(){
  /*
     USART0_RX 从外面接收数据功能  外部设备 ---》 内存
     DMA1  CH5  100
  */
  // 开启DMA时钟
  rcu_periph_clock_enable(RCU_DMA1);
  // 配置DMA
  //dma_deinit(DMA1,DMA_CH5);
  dma_deinit(RX_DMA_PERIPH);
  // 数据结构初始化
  dma_single_data_parameter_struct init_data;
  dma_single_data_para_struct_init(&init_data);
  
  // 配置具体的DMA参数
 /* set the DMA struct with the default values */
  init_data.direction           = DMA_PERIPH_TO_MEMORY;
  
  // 外设的配置
  init_data.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
  init_data.periph_addr         = (uint32_t)&USART_DATA(USART0);// 串口发送数据的地址
  init_data.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
  
  // 内存的配置
  init_data.memory0_addr        = (uint32_t)g_recv_buffer;  // 后续动态填写
  init_data.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
  init_data.number              = RECV_BUFFER_LEN;

  // 无需循环
  init_data.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
  // 优先级的配置
  init_data.priority            = DMA_PRIORITY_LOW;
 
  // 完成DMA发送的基础配置 DMA1  CH5  100
  dma_single_data_mode_init(RX_DMA_PERIPH,&init_data);
  // 填写子通道编号 
  dma_channel_subperipheral_select(RX_DMA_PERIPH, DMA_SUBPERI4);
  
  // 设置DMA开始干活
  dma_channel_enable(RX_DMA_PERIPH);
}

static void dma_tx_config(){
  rcu_periph_clock_enable(RCU_DMA1);
	
	dma_deinit(DMA_TX_PERIPH);
	
	dma_single_data_parameter_struct dma_init;
	
	dma_single_data_para_struct_init(&dma_init);
	
	  dma_init.direction           = DMA_MEMORY_TO_PERIPH;
	 /* set the DMA struct with the default values */
    dma_init.memory0_addr        = 0;
    dma_init.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
    dma_init.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
	
	  dma_init.periph_addr         = (uint32_t)&USART_DATA(USART0);
    dma_init.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
	
    dma_init.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
    
    dma_init.number              = 0U;
    dma_init.priority            = DMA_PRIORITY_LOW;
	
	 dma_single_data_mode_init(DMA_TX_PERIPH,&dma_init);
	
   dma_channel_subperipheral_select(DMA_TX_PERIPH,DMA_SUBPERI4);
}

void usart_dma_init(){
  
	dma_rx_config();
	dma_tx_config();
  // 1. 开启引脚时钟
  rcu_periph_clock_enable(RCU_GPIOA);  
  // 2. 开启串口时钟
  rcu_periph_clock_enable(RCU_USART0); 
  
  // 3. 配置引脚复用功能
  gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_9);
  gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_10);
  
  gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_9);
  gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_10);
  
  gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_MAX,GPIO_PIN_9);
  gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_MAX,GPIO_PIN_10);
  // ----------------------------------------------------
  
   // 4. 配置串口功能
  usart_deinit(USART0);
  // 5. 设置传输速率 
  usart_baudrate_set(USART0,115200);
  // 6. 启用发送功能
  usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
  // 启用串口接收的功能
  usart_receive_config(USART0,USART_RECEIVE_ENABLE);
  
  /* 启用串口0的中断请求*/
   nvic_irq_enable(USART0_IRQn, 0, 0);
   // 启用中断,监听标志位USART_INT_RBNE 
   // /*!< Read data Buffer Not Empty interrupt and overrun error interrupt */
   // 当接收数据的缓冲区不为空的时候, 就进入到中断函数中
   //usart_interrupt_enable(USART0, USART_INT_RBNE);
   //usart_interrupt_enable(USART0, USART_INT_IDLE);
   
  // 7. 启用串口
  //usart_enable(USART0);
  
  // =====================配置与DMA 相关的内容==================
  usart_dma_receive_config(USART0,USART_RECEIVE_DMA_ENABLE);
  usart_dma_transmit_config(USART0,USART_TRANSMIT_DMA_ENABLE);
  
  // 开启空闲中断
  usart_interrupt_enable(USART0, USART_INT_IDLE);
  // 开启串口
  usart_enable(USART0);

}

void usart0_send_dma_data(uint8_t* data,int len){
    dma_memory_address_config(DMA_TX_PERIPH,DMA_MEMORY_0,(uint32_t)data);
	
	  dma_transfer_number_config(DMA_TX_PERIPH,len);
	  
	  dma_channel_enable(DMA_TX_PERIPH);
	
	 while(RESET==dma_flag_get(DMA_TX_PERIPH,DMA_INT_FLAG_FTF));
	
   dma_flag_clear(DMA_TX_PERIPH,DMA_INT_FLAG_FTF);

}
void USART0_IRQHandler(void)
{
  if(SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)){
      // 读出来的是没有意义的， 这个地方主要用于清空标志位
      usart_data_receive(USART0);
      
      // 获取还未搬运的数据量 [1,2,3,4,5,0,0,0,0,0] , 剩余空间
      uint32_t cnt = dma_transfer_number_get(RX_DMA_PERIPH);
      
      // 计算实际搬运的数据
      recv_cnt = RECV_BUFFER_LEN - cnt;
      
      
      printf("recv_cnt:%d %s\r\n",recv_cnt,g_recv_buffer);
      
      // 清空一下DMA状态传输完成的状态
      dma_channel_disable(RX_DMA_PERIPH);
      dma_flag_clear(RX_DMA_PERIPH,DMA_FLAG_FTF);
  	  dma_channel_enable(RX_DMA_PERIPH);
		
  }
}
int fputc(int ch, FILE *f)  // 定义fputc,因为printf底层会调用这个函数
{   
    // 串口发送功能                0-255     
    //usart_data_transmit(USART0,ch);
  // TBE : Transmit data Buffer Empty 发送数据的缓冲区为空
    // 判断是否发送完成, 等待这一个字节数据发送完成
    //while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	   uint8_t data= ch;
	   usart0_send_dma_data(&data,1);
    return ch;
}
#endif