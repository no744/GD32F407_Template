#include "SPI2.h"
#if USE_SPI2
#include "stdio.h"

#if SPI2_SOFT

#define SCL(bit)	gpio_bit_write(SPI2_SCL_PORT, SPI2_SCL_PIN, bit ? SET : RESET)
#define MOSI(bit)	gpio_bit_write(SPI2_MOSI_PORT, SPI2_MOSI_PIN, bit ? SET : RESET)
#define MISO()		gpio_input_bit_get(SPI2_MISO_PORT,SPI2_MISO_PIN)

/****************** ��ʵ�� *********************/
void SPI2_init() {

    // CLK, MOSI, MISO, (CS ���豸ҵ��ҹ�)

    // CLK
    rcu_periph_clock_enable(SPI2_SCL_RCU);
    gpio_mode_set(SPI2_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI2_SCL_PIN);
    gpio_output_options_set(SPI2_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI2_SCL_PIN);
    // MOSI
    rcu_periph_clock_enable(SPI2_MOSI_RCU);
    gpio_mode_set(SPI2_MOSI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI2_MOSI_PIN);
    gpio_output_options_set(SPI2_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI2_MOSI_PIN);

    // MISO
    rcu_periph_clock_enable(SPI2_MISO_RCU);
    gpio_mode_set(SPI2_MISO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SPI2_MISO_PIN);


    gpio_bit_set(SPI2_SCL_PORT, SPI2_SCL_PIN);
    gpio_bit_set(SPI2_MOSI_PORT, SPI2_MOSI_PIN);
    gpio_bit_set(SPI2_MISO_PORT, SPI2_MISO_PIN);
}


void SPI2_write(uint8_t dat) {
    uint8_t i;
    for(i=0; i<8; i++)
    {
        SCL(0);
        MOSI(dat&0x80);
        SCL(1);
        dat<<=1;
    }
}

uint8_t SPI2_read() {
    uint8_t i,read=0;
    for(i=0; i<8; i++)
    {
        SCL(0);
        read<<=1;
        if(MISO())
        {
            read++;
        }
        SCL(1);
    }
    return read;
}


uint8_t SPI2_read_write(uint8_t dat){
	
    uint8_t i,read=0;
    for(i=0; i<8; i++)
    {
		// write
        SCL(0);
        MOSI(dat&0x80);
        dat<<=1;
		
		// read
        read<<=1;
        if(MISO())
        {
            read++;
        }
        SCL(1);
    }
	
    return read;
}


#else

/****************** Ӳ��ʵ�� *********************/
void SPI2_init(){
	//AF ---------------------------------------------------------------
	
    // CLK, MOSI, MISO, (CS ���豸ҵ��ҹ�)

    // CLK
    rcu_periph_clock_enable(SPI2_SCL_RCU);
    gpio_mode_set(SPI2_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI2_SCL_PIN);
    gpio_output_options_set(SPI2_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI2_SCL_PIN);
    gpio_af_set(SPI2_SCL_PORT, GPIO_AF_6, SPI2_SCL_PIN);
    
    // MISO
    rcu_periph_clock_enable(SPI2_MISO_RCU);
    gpio_mode_set(SPI2_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI2_MISO_PIN);
    gpio_af_set(SPI2_MISO_PORT, GPIO_AF_6, SPI2_MISO_PIN);
	
  
	// MOSI
    rcu_periph_clock_enable(SPI2_MOSI_RCU);
    gpio_mode_set(SPI2_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI2_MOSI_PIN);
    gpio_output_options_set(SPI2_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI2_MOSI_PIN);
    gpio_af_set(SPI2_MOSI_PORT, GPIO_AF_5, SPI2_MOSI_PIN);
	

	
	// ������λ�ͼ��Գ�ʼ ��ƽ�ߵ� 
    gpio_bit_set(SPI2_SCL_PORT, SPI2_SCL_PIN);
    gpio_bit_set(SPI2_MOSI_PORT, SPI2_MOSI_PIN);
    gpio_bit_set(SPI2_MISO_PORT, SPI2_MISO_PIN);
	
	
	// rcu ---------------------------------------------------------------
	rcu_periph_clock_enable(RCU_SPI2);
	
	
	// spi ---------------------------------------------------------------
	spi_parameter_struct spi_struct;	
	/* initialize the parameters of SPI struct with default values */
	spi_struct_para_init(&spi_struct);
	
    /* configure the structure with default value */
    spi_struct.device_mode          = SPI_MASTER;	// ����ģʽ
    spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;	// default
    spi_struct.frame_size           = SPI2_FRAME_SIZE;		// default
    spi_struct.nss                  = SPI_NSS_SOFT;	// ��Ƭѡ
    spi_struct.clock_polarity_phase = SPI2_CLOCK_POLARITY_PHASE;	// (1, 1)
    spi_struct.prescale             = SPI2_PRESCALE;	// ��Ƶ, 120M/PSC Խ�󷢵�Խ��
    spi_struct.endian               = SPI2_ENDIAN;			// default
	
	/* initialize SPI parameter */
	spi_init(SPI2, &spi_struct);
	/* enable SPI */
	spi_enable(SPI2);
}

void SPI2_write(uint8_t data){
	// Ԥ��Ƶϵ�� ���
	
	// ѭ���ȴ�������Ϊ��
	while(spi_i2s_flag_get(SPI2, SPI_FLAG_TBE) == RESET);
	// ֪ͨ�����·�ɻ�
	spi_i2s_data_transmit(SPI2, data);
	
	// ѭ���ȴ����ղ�Ϊ��
	while(spi_i2s_flag_get(SPI2, SPI_FLAG_RBNE) == RESET);
	// ֪ͨ�����·�ɻ�
	spi_i2s_data_receive(SPI2);
}

uint8_t SPI2_read(){	
	// ѭ���ȴ�������Ϊ��
	while(spi_i2s_flag_get(SPI2, SPI_FLAG_TBE) == RESET);
	// ֪ͨ�����·�ɻ�
	spi_i2s_data_transmit(SPI2, 0x00);
	
	// ѭ���ȴ����ղ�Ϊ��
	while(spi_i2s_flag_get(SPI2, SPI_FLAG_RBNE) == RESET);
	// ֪ͨ�����·�ɻ�
	return spi_i2s_data_receive(SPI2);
}

uint8_t SPI2_read_write(uint8_t data){	
	// ѭ���ȴ�������Ϊ��
	while(spi_i2s_flag_get(SPI2, SPI_FLAG_TBE) == RESET);
	// ֪ͨ�����·�ɻ�
	spi_i2s_data_transmit(SPI2, data);
	
  
	// ѭ���ȴ����ղ�Ϊ��
	while(spi_i2s_flag_get(SPI2, SPI_FLAG_RBNE) == RESET);
	// ֪ͨ�����·�ɻ�
	return spi_i2s_data_receive(SPI2);
}
#endif

#endif