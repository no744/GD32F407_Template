#include "Spi0.h"
#if USE_SPI0


#if SPI0_SOFT

#define SCL(bit)	gpio_bit_write(SPI0_SCL_PORT, SPI0_SCL_PIN, bit ? SET : RESET)
#define MOSI(bit)	gpio_bit_write(SPI0_MOSI_PORT, SPI0_MOSI_PIN, bit ? SET : RESET)
#define MISO()		gpio_input_bit_get(SPI0_MISO_PORT,SPI0_MISO_PIN)

/****************** 软实现 *********************/
void SPI0_init() {

    // CLK, MOSI, MISO, (CS 和设备业务挂钩)

    // CLK
    rcu_periph_clock_enable(SPI0_SCL_RCU);
    gpio_mode_set(SPI0_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI0_SCL_PIN);
    gpio_output_options_set(SPI0_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_SCL_PIN);
    // MOSI
    rcu_periph_clock_enable(SPI0_MOSI_RCU);
    gpio_mode_set(SPI0_MOSI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPI0_MOSI_PIN);
    gpio_output_options_set(SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_MOSI_PIN);

    // MISO
    rcu_periph_clock_enable(SPI0_MISO_RCU);
    gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);


    gpio_bit_set(SPI0_SCL_PORT, SPI0_SCL_PIN);
    gpio_bit_set(SPI0_MOSI_PORT, SPI0_MOSI_PIN);
    gpio_bit_set(SPI0_MISO_PORT, SPI0_MISO_PIN);
}


void SPI0_write(uint8_t dat) {
    uint8_t i;
    for(i=0; i<8; i++)
    {
        SCL(0);
        MOSI(dat&0x80);
        SCL(1);
        dat<<=1;
    }
}

uint8_t SPI0_read() {
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


uint8_t SPI0_read_write(uint8_t dat){
	
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

/****************** 硬件实现 *********************/
void SPI0_init(){
	//AF ---------------------------------------------------------------
	
    // CLK, MOSI, MISO, (CS 和设备业务挂钩)

    // CLK
    rcu_periph_clock_enable(SPI0_SCL_RCU);
    gpio_mode_set(SPI0_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI0_SCL_PIN);
    gpio_output_options_set(SPI0_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_SCL_PIN);
    gpio_af_set(SPI0_SCL_PORT, GPIO_AF_5, SPI0_SCL_PIN);
	
	// MOSI
    rcu_periph_clock_enable(SPI0_MOSI_RCU);
    gpio_mode_set(SPI0_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI0_MOSI_PIN);
    gpio_output_options_set(SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI0_MOSI_PIN);
    gpio_af_set(SPI0_MOSI_PORT, GPIO_AF_5, SPI0_MOSI_PIN);
	
    // MISO
    rcu_periph_clock_enable(SPI0_MISO_RCU);
    gpio_mode_set(SPI0_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI0_MISO_PIN);
    gpio_af_set(SPI0_MISO_PORT, GPIO_AF_5, SPI0_MISO_PIN);
	
	
	// 根据相位和极性初始 电平高低 
    gpio_bit_set(SPI0_SCL_PORT, SPI0_SCL_PIN);
    gpio_bit_set(SPI0_MOSI_PORT, SPI0_MOSI_PIN);
    gpio_bit_set(SPI0_MISO_PORT, SPI0_MISO_PIN);
	
	
	// rcu ---------------------------------------------------------------
	rcu_periph_clock_enable(RCU_SPI0);
	
	
	// spi ---------------------------------------------------------------
	spi_parameter_struct spi_struct;	
	/* initialize the parameters of SPI struct with default values */
	spi_struct_para_init(&spi_struct);
	
    /* configure the structure with default value */
    spi_struct.device_mode          = SPI_MASTER;	// 主机模式
    spi_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;	// default
    spi_struct.frame_size           = SPI0_FRAME_SIZE;		// default
    spi_struct.nss                  = SPI_NSS_SOFT;	// 软片选
    spi_struct.clock_polarity_phase = SPI0_CLOCK_POLARITY_PHASE;	// (1, 1)
    spi_struct.prescale             = SPI0_PRESCALE;	// 分频, 120M/PSC 越大发的越慢
    spi_struct.endian               = SPI0_ENDIAN;			// default
	
	/* initialize SPI parameter */
	spi_init(SPI0, &spi_struct);
	/* enable SPI */
	spi_enable(SPI0);
}

void SPI0_write(uint8_t data){
	// 预分频系数 相关
	
	// 循环等待发送区为空
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET);
	// 通知外设电路干活
	spi_i2s_data_transmit(SPI0, data);
	
	// 循环等待接收不为空
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET);
	// 通知外设电路干活
	spi_i2s_data_receive(SPI0);
}

uint8_t SPI0_read(){	
	// 循环等待发送区为空
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET);
	// 通知外设电路干活
	spi_i2s_data_transmit(SPI0, 0x00);
	
	// 循环等待接收不为空
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET);
	// 通知外设电路干活
	return spi_i2s_data_receive(SPI0);
}

uint8_t SPI0_read_write(uint8_t data){	
	// 循环等待发送区为空
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_TBE) == RESET);
	// 通知外设电路干活
	spi_i2s_data_transmit(SPI0, data);
	
	// 循环等待接收不为空
	while(spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE) == RESET);
	// 通知外设电路干活
	return spi_i2s_data_receive(SPI0);
}
#endif

#endif