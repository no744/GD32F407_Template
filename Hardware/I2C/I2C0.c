#include "I2C0.h"

#if I2C0_SOFT

#if I2C0_SPEED < 200000
#define DELAY()		delay_1us(5)
#else
#define DELAY()		delay_1us(2)
#endif

#define SCL(bit)	gpio_bit_write(I2C0_SCL_PORT, I2C0_SCL_PIN, bit == 1 ? SET : RESET)
#define SDA(bit)	gpio_bit_write(I2C0_SDA_PORT, I2C0_SDA_PIN, bit == 1 ? SET : RESET)

#define SDA_IN() 	gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, I2C0_SDA_PIN)
#define SDA_OUT() 	gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SDA_PIN)

#define SDA_STATE()	gpio_input_bit_get(I2C0_SDA_PORT, I2C0_SDA_PIN)

static void start();
static void send(uint8_t data);
static uint8_t wait_ack();
static uint8_t recv();
static void send_ack();
static void send_nack();
static void stop();

void I2C0_init() {
    // 寄存器，软实现：CPU实现
    //  复用， 硬实现：电路实现

    // I2C速率：100k，400k，1M，5M 比特
    // 100k/1s => 100000bit/1000000us => 1bit/10us 高低电平5us
    // 400k/1s => 400000bit/1000000us => 4bit/10us 高低电平2us

    // I2C0
    rcu_periph_clock_enable(I2C0_SCL_RCU);
    gpio_mode_set(I2C0_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SCL_PIN);
    gpio_output_options_set(I2C0_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C0_SCL_PIN);

    rcu_periph_clock_enable(I2C0_SDA_RCU);
    gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SDA_PIN);
    gpio_output_options_set(I2C0_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C0_SDA_PIN);

}

uint8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {

    // 开始
    start();
    // 发送设备地址addr
    send(addr << 1);
    // 等待响应
    if(wait_ack())return I2C_ERR_DEVICE_ADDR;
    // 发送寄存器地址reg
    send(reg);
    // 等待响应
    if(wait_ack())return I2C_ERR_REGISTER;

    uint32_t i;
    for(i = 0; i < len; i++) {
        // 发送数据
        send(data[i]);
        // 等待响应
		if(wait_ack())return I2C_ERR_WRITE;
    }

    // 停止
    stop();
	
	return I2C_RESULT_OK;

}

uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len){

    // 开始
    start();
    // 发送设备地址addr
    send(addr << 1);
    // 等待响应
    if(wait_ack())return I2C_ERR_DEVICE_ADDR;
    // 发送寄存器地址reg
    send(reg);
    // 等待响应
    if(wait_ack())return I2C_ERR_REGISTER;

    uint32_t i;
    for(i = 0; i < len; i++) {
        // 发送数据
        send(data[i * offset]);
        // 等待响应
		if(wait_ack())return I2C_ERR_WRITE;
    }

    // 停止
    stop();
	
	return I2C_RESULT_OK;

}

uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	uint8_t write_addr = addr << 1;
	uint8_t read_addr  = (addr << 1) | 1;
	// 开始
	start();
	// 发送设备地址(写地址)
	send(write_addr);
	// 等待响应
	if(wait_ack()) return I2C_ERR_DEVICE_ADDR;
	// 发送寄存器地址
	send(reg);
	// 等待响应
	if(wait_ack()) return I2C_ERR_REGISTER;
	// 开始
	start();
	
	// 发送设备地址(读地址)
	send(read_addr);
	// 等待响应
	if(wait_ack()) return I2C_ERR_READ;
	
	/*********接收数据*********/
	// for n
	for(uint32_t i = 0; i < len; i++){
		// 接收数据
		data[i] = recv();		
		if (i < len - 1){
			// 发送ACK响应		
			send_ack();
		}else {
			// 发送NACK响应
			send_nack();
		}
	}
	/**************************/
	
	// 结束
	stop();
	
	return I2C_RESULT_OK;
}

static uint8_t recv(){
	uint8_t i = 8;
	uint8_t data = 0x00;
	// 主设备释放SDA控制权
	SDA_IN();
	while(i--){
		SCL(0); 	// 等待从设备准备数据
		DELAY();	
		
		SCL(1);		// 设置数据有效性
		// 0000 0000 => 1111 1111
		// 0000 0001
		// 0000 0011
		// 0000 0111
		// 0000 1111
		data <<= 1;
		if(SDA_STATE()) data++;
				
		DELAY();	
	}
	SCL(0);	// 最后一次低电平
	return data;
}

static void send_ack(){
	// 主设备输出
	SDA_OUT();
	
	SDA(0);
	DELAY();
	
	SCL(1);
	DELAY();
	
	SCL(0);
	DELAY();

}
static void send_nack(){
	// 主设备输出
	SDA_OUT();
	
	SDA(1);
	DELAY();
	
	SCL(1);
	DELAY();
	
	SCL(0);
	DELAY();

}
static void start() {
    // 获取SDA控制权
    SDA_OUT();
	
    // 开始
    SDA(1);
    DELAY();

    SCL(1);
    DELAY();

    SDA(0);
    DELAY();

    SCL(0);
    DELAY();

}
static void send(uint8_t data) {
    // 获取SDA控制权
    SDA_OUT();
    // 8个bit
    // 先发高位
    // 1101 0010
    uint8_t i;
    for(i = 0; i < 8; i++) {
        if(data & 0x80) {
            SDA(1);
        } else {
            SDA(0);
        }
        DELAY();

        SCL(1);
        DELAY();
        SCL(0);
        DELAY();

        data <<= 1;
    }

}
static uint8_t wait_ack() {
	int8_t retry = 3;
    // 主到从：send，主设备控制pin，修改pin电平高低
    // 从到主：wait，从设备控制pin，修改pin电平高低

    // SDA初始化, 拉高
    SDA(1);
    DELAY();
	// SCL初始化, 拉高
    SCL(1);
    DELAY();

    // 将SCL设置为输入模式，交出SDA控制权, 等待被拉低
    SDA_IN();
    DELAY();
		
    // 检测SDA电平变化
//    if (SDA_STATE() == RESET) {
//        // ACK成功, 恢复SCL为低电平
//        SCL(0);
//		// 获取SDA控制权
//		SDA_OUT();
//		DELAY();
//    } else {
//        // ACK失败，STOP
//        stop();
//		return 1;
//    }

	while(SDA_STATE() == SET && retry > 0){
		// 重试
		retry--;
		DELAY();
	}
	
	if (retry <= 0){
		stop();
		return 1;
	}else {		
        // ACK成功, 恢复SCL为低电平
        SCL(0);
		// 获取SDA控制权
		SDA_OUT();
	}
	
	return 0;

}


static void stop() {
    // 获取SDA控制权
    SDA_OUT();

    // 结束
    SDA(0);
    SCL(0);

    SCL(1);
    DELAY();

    SDA(1);
    DELAY();

}

#else

#define i2cx	I2C0

void I2C0_init() {
    // I2C的速率：100k 400k 1M 5M
    // 主(240M) 从(1M)
    // 100k：1000000us 100000bit, 10us 1bit, 5us
    // 400k：1000000us 400000bit, 10us 1bit, 2us

    // 复用，硬实现，电路实现
    ///////////// GPIO
    // SCL	PB6
    rcu_periph_clock_enable(I2C0_SCL_RCU);
    gpio_mode_set(I2C0_SCL_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, I2C0_SCL_PIN);
    gpio_output_options_set(I2C0_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C0_SCL_PIN);
    gpio_af_set(I2C0_SCL_PORT, GPIO_AF_4, I2C0_SCL_PIN);

    // SDA	PB7
    rcu_periph_clock_enable(I2C0_SDA_RCU);
    gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, I2C0_SDA_PIN);
    gpio_output_options_set(I2C0_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C0_SDA_PIN);
    gpio_af_set(I2C0_SDA_PORT, GPIO_AF_4, I2C0_SDA_PIN);

    //////////// I2C电路
    rcu_periph_clock_enable(RCU_I2C0);

    i2c_deinit(i2cx);
    // I2C速率配置
    i2c_clock_config(i2cx, I2C0_SPEED, I2C_DTCY_2);

    // 使能i2c 0000 000r/w
    i2c_mode_addr_config(i2cx, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x00);
    i2c_enable(i2cx);

    // i2c ack enable,
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);
}


static uint8_t I2C_wait(uint32_t flag) {
    uint16_t TIMEOUT = 50000;
    uint16_t cnt = 0;

    while(!i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
    return 0;
}

static uint8_t I2C_waitn(uint32_t flag) {
    uint16_t TIMEOUT = 50000;
    uint16_t cnt = 0;

    while(i2c_flag_get(i2cx, flag)) {
        cnt++;
        if(cnt > TIMEOUT) return 1;
    }
    return 0;
}


uint8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {
    // addr设备地址， 0000 000(r/w)
    uint8_t waddr = addr << 1;
    // addr 是b1-b7
    /************* start ***********************/
    // 等待I2C闲置
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    // start
    i2c_start_on_bus(i2cx);

    // 等待I2C主设备成功发送起始信号
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    /************* device address **************/
    // 发送设备地址(写地址)
    i2c_master_addressing(i2cx, waddr, I2C_TRANSMITTER);
    // 等待地址发送完成
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************ register address ************/
    // 寄存器地址
    // 等待发送数据缓冲区为空
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // 发送数据
    i2c_data_transmit(i2cx, reg);
    // 等待数据发送完成
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /***************** data ******************/
    // 发送数据
    uint32_t i;
    for(i = 0; i < len; i++) {
        uint8_t d = data[i];

        // 等待发送数据缓冲区为空
        if(I2C_wait(I2C_FLAG_TBE)) return 6;

        // 发送数据
        i2c_data_transmit(i2cx, d);

        // 等待数据发送完成
        if(I2C_wait(I2C_FLAG_BTC)) return 7;
    }

    /***************** stop ********************/
    // stop
    i2c_stop_on_bus(i2cx);

    return I2C_RESULT_OK;// 成功
}

uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len) {
    // addr设备地址， 0000 000(r/w)
    uint8_t waddr = addr << 1;
    // addr 是b1-b7
    /************* start ***********************/
    // 等待I2C闲置
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    // start
    i2c_start_on_bus(i2cx);

    // 等待I2C主设备成功发送起始信号
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    /************* device address **************/
    // 发送设备地址(写地址)
    i2c_master_addressing(i2cx, waddr, I2C_TRANSMITTER);
    // 等待地址发送完成
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************ register address ************/
    // 寄存器地址
    // 等待发送数据缓冲区为空
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // 发送数据
    i2c_data_transmit(i2cx, reg);
    // 等待数据发送完成
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /***************** data ******************/
    // 发送数据
    uint32_t i;
    for(i = 0; i < len; i++) {
        uint8_t d = data[i * offset];

        // 等待发送数据缓冲区为空
        if(I2C_wait(I2C_FLAG_TBE)) return 6;

        // 发送数据
        i2c_data_transmit(i2cx, d);

        // 等待数据发送完成
        if(I2C_wait(I2C_FLAG_BTC)) return 7;
    }

    /***************** stop ********************/
    // stop
    i2c_stop_on_bus(i2cx);

    return I2C_RESULT_OK;// 成功
}
uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {
    uint8_t waddr = addr << 1;
    uint8_t raddr = (addr << 1) | 1;
    // 开始
    /************* start ***********************/
    // 等待I2C空闲
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    // 发送启动信号
    i2c_start_on_bus(i2cx);
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    // 发送设备地址(写地址)
    /************* device address **************/
    // 发送从设备地址
    i2c_master_addressing(i2cx, waddr, I2C_TRANSMITTER);

    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    // 发送寄存器地址
    /********** register address **************/
    // 等待发送缓冲区
    if(I2C_wait(I2C_FLAG_TBE)) return 4;

    // 发送寄存器地址
    i2c_data_transmit(i2cx, reg);

    // 等待发送数据完成
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /************* start ***********************/
    // 发送再启动信号
    i2c_start_on_bus(i2cx);
    if(I2C_wait(I2C_FLAG_SBSEND)) return 7;


    /************* device address **************/
    // 发送从设备地址
    i2c_master_addressing(i2cx, raddr, I2C_RECEIVER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 8;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************* data **************/
    //ack
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);// send ack
    i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);// 接收一个数据之后，就自动发送ack
    if(I2C_wait((I2C_CTL0(i2cx) & I2C_CTL0_ACKEN))) return 11;

    // 读取数据
    uint8_t i;
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            // 在读取最后一个字节之前，禁用ACK，并发送停止信号
            // 配置自动NACK
            i2c_ack_config(i2cx, I2C_ACK_DISABLE);
			if(I2C_wait((I2C_CTL0(i2cx) & I2C_CTL0_ACKEN))) return 12;
        }
		
        // 等待ACK数据发送完成
        // 等待接收缓冲区
        if(I2C_wait(I2C_FLAG_RBNE)) return 10;
        data[i] = i2c_data_receive(i2cx);
    }
    /***************** stop ********************/
    i2c_stop_on_bus(i2cx);

    return 0;
}
#endif