#include "usart0_dma.h"
#include <stdio.h>
//����usart��װ
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
     USART0_RX ������������ݹ���  �ⲿ�豸 ---�� �ڴ�
     DMA1  CH5  100
  */
  // ����DMAʱ��
  rcu_periph_clock_enable(RCU_DMA1);
  // ����DMA
  //dma_deinit(DMA1,DMA_CH5);
  dma_deinit(RX_DMA_PERIPH);
  // ���ݽṹ��ʼ��
  dma_single_data_parameter_struct init_data;
  dma_single_data_para_struct_init(&init_data);
  
  // ���þ����DMA����
 /* set the DMA struct with the default values */
  init_data.direction           = DMA_PERIPH_TO_MEMORY;
  
  // ���������
  init_data.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
  init_data.periph_addr         = (uint32_t)&USART_DATA(USART0);// ���ڷ������ݵĵ�ַ
  init_data.periph_inc          = DMA_PERIPH_INCREASE_DISABLE;
  
  // �ڴ������
  init_data.memory0_addr        = (uint32_t)g_recv_buffer;  // ������̬��д
  init_data.memory_inc          = DMA_MEMORY_INCREASE_ENABLE;
  init_data.number              = RECV_BUFFER_LEN;

  // ����ѭ��
  init_data.circular_mode       = DMA_CIRCULAR_MODE_DISABLE;
  // ���ȼ�������
  init_data.priority            = DMA_PRIORITY_LOW;
 
  // ���DMA���͵Ļ������� DMA1  CH5  100
  dma_single_data_mode_init(RX_DMA_PERIPH,&init_data);
  // ��д��ͨ����� 
  dma_channel_subperipheral_select(RX_DMA_PERIPH, DMA_SUBPERI4);
  
  // ����DMA��ʼ�ɻ�
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
  // 1. ��������ʱ��
  rcu_periph_clock_enable(RCU_GPIOA);  
  // 2. ��������ʱ��
  rcu_periph_clock_enable(RCU_USART0); 
  
  // 3. �������Ÿ��ù���
  gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_9);
  gpio_af_set(GPIOA,GPIO_AF_7,GPIO_PIN_10);
  
  gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_9);
  gpio_mode_set(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO_PIN_10);
  
  gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_MAX,GPIO_PIN_9);
  gpio_output_options_set(GPIOA,GPIO_OTYPE_PP,GPIO_OSPEED_MAX,GPIO_PIN_10);
  // ----------------------------------------------------
  
   // 4. ���ô��ڹ���
  usart_deinit(USART0);
  // 5. ���ô������� 
  usart_baudrate_set(USART0,115200);
  // 6. ���÷��͹���
  usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
  // ���ô��ڽ��յĹ���
  usart_receive_config(USART0,USART_RECEIVE_ENABLE);
  
  /* ���ô���0���ж�����*/
   nvic_irq_enable(USART0_IRQn, 0, 0);
   // �����ж�,������־λUSART_INT_RBNE 
   // /*!< Read data Buffer Not Empty interrupt and overrun error interrupt */
   // ���������ݵĻ�������Ϊ�յ�ʱ��, �ͽ��뵽�жϺ�����
   //usart_interrupt_enable(USART0, USART_INT_RBNE);
   //usart_interrupt_enable(USART0, USART_INT_IDLE);
   
  // 7. ���ô���
  //usart_enable(USART0);
  
  // =====================������DMA ��ص�����==================
  usart_dma_receive_config(USART0,USART_RECEIVE_DMA_ENABLE);
  usart_dma_transmit_config(USART0,USART_TRANSMIT_DMA_ENABLE);
  
  // ���������ж�
  usart_interrupt_enable(USART0, USART_INT_IDLE);
  // ��������
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
      // ����������û������ģ� ����ط���Ҫ������ձ�־λ
      usart_data_receive(USART0);
      
      // ��ȡ��δ���˵������� [1,2,3,4,5,0,0,0,0,0] , ʣ��ռ�
      uint32_t cnt = dma_transfer_number_get(RX_DMA_PERIPH);
      
      // ����ʵ�ʰ��˵�����
      recv_cnt = RECV_BUFFER_LEN - cnt;
      
      
      printf("recv_cnt:%d %s\r\n",recv_cnt,g_recv_buffer);
      
      // ���һ��DMA״̬������ɵ�״̬
      dma_channel_disable(RX_DMA_PERIPH);
      dma_flag_clear(RX_DMA_PERIPH,DMA_FLAG_FTF);
  	  dma_channel_enable(RX_DMA_PERIPH);
		
  }
}
int fputc(int ch, FILE *f)  // ����fputc,��Ϊprintf�ײ������������
{   
    // ���ڷ��͹���                0-255     
    //usart_data_transmit(USART0,ch);
  // TBE : Transmit data Buffer Empty �������ݵĻ�����Ϊ��
    // �ж��Ƿ������, �ȴ���һ���ֽ����ݷ������
    //while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	   uint8_t data= ch;
	   usart0_send_dma_data(&data,1);
    return ch;
}
#endif