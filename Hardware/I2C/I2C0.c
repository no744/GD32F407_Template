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
    // �Ĵ�������ʵ�֣�CPUʵ��
    //  ���ã� Ӳʵ�֣���·ʵ��

    // I2C���ʣ�100k��400k��1M��5M ����
    // 100k/1s => 100000bit/1000000us => 1bit/10us �ߵ͵�ƽ5us
    // 400k/1s => 400000bit/1000000us => 4bit/10us �ߵ͵�ƽ2us

    // I2C0
    rcu_periph_clock_enable(I2C0_SCL_RCU);
    gpio_mode_set(I2C0_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SCL_PIN);
    gpio_output_options_set(I2C0_SCL_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C0_SCL_PIN);

    rcu_periph_clock_enable(I2C0_SDA_RCU);
    gpio_mode_set(I2C0_SDA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, I2C0_SDA_PIN);
    gpio_output_options_set(I2C0_SDA_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_MAX, I2C0_SDA_PIN);

}

uint8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {

    // ��ʼ
    start();
    // �����豸��ַaddr
    send(addr << 1);
    // �ȴ���Ӧ
    if(wait_ack())return I2C_ERR_DEVICE_ADDR;
    // ���ͼĴ�����ַreg
    send(reg);
    // �ȴ���Ӧ
    if(wait_ack())return I2C_ERR_REGISTER;

    uint32_t i;
    for(i = 0; i < len; i++) {
        // ��������
        send(data[i]);
        // �ȴ���Ӧ
		if(wait_ack())return I2C_ERR_WRITE;
    }

    // ֹͣ
    stop();
	
	return I2C_RESULT_OK;

}

uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len){

    // ��ʼ
    start();
    // �����豸��ַaddr
    send(addr << 1);
    // �ȴ���Ӧ
    if(wait_ack())return I2C_ERR_DEVICE_ADDR;
    // ���ͼĴ�����ַreg
    send(reg);
    // �ȴ���Ӧ
    if(wait_ack())return I2C_ERR_REGISTER;

    uint32_t i;
    for(i = 0; i < len; i++) {
        // ��������
        send(data[i * offset]);
        // �ȴ���Ӧ
		if(wait_ack())return I2C_ERR_WRITE;
    }

    // ֹͣ
    stop();
	
	return I2C_RESULT_OK;

}

uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len){
	uint8_t write_addr = addr << 1;
	uint8_t read_addr  = (addr << 1) | 1;
	// ��ʼ
	start();
	// �����豸��ַ(д��ַ)
	send(write_addr);
	// �ȴ���Ӧ
	if(wait_ack()) return I2C_ERR_DEVICE_ADDR;
	// ���ͼĴ�����ַ
	send(reg);
	// �ȴ���Ӧ
	if(wait_ack()) return I2C_ERR_REGISTER;
	// ��ʼ
	start();
	
	// �����豸��ַ(����ַ)
	send(read_addr);
	// �ȴ���Ӧ
	if(wait_ack()) return I2C_ERR_READ;
	
	/*********��������*********/
	// for n
	for(uint32_t i = 0; i < len; i++){
		// ��������
		data[i] = recv();		
		if (i < len - 1){
			// ����ACK��Ӧ		
			send_ack();
		}else {
			// ����NACK��Ӧ
			send_nack();
		}
	}
	/**************************/
	
	// ����
	stop();
	
	return I2C_RESULT_OK;
}

static uint8_t recv(){
	uint8_t i = 8;
	uint8_t data = 0x00;
	// ���豸�ͷ�SDA����Ȩ
	SDA_IN();
	while(i--){
		SCL(0); 	// �ȴ����豸׼������
		DELAY();	
		
		SCL(1);		// ����������Ч��
		// 0000 0000 => 1111 1111
		// 0000 0001
		// 0000 0011
		// 0000 0111
		// 0000 1111
		data <<= 1;
		if(SDA_STATE()) data++;
				
		DELAY();	
	}
	SCL(0);	// ���һ�ε͵�ƽ
	return data;
}

static void send_ack(){
	// ���豸���
	SDA_OUT();
	
	SDA(0);
	DELAY();
	
	SCL(1);
	DELAY();
	
	SCL(0);
	DELAY();

}
static void send_nack(){
	// ���豸���
	SDA_OUT();
	
	SDA(1);
	DELAY();
	
	SCL(1);
	DELAY();
	
	SCL(0);
	DELAY();

}
static void start() {
    // ��ȡSDA����Ȩ
    SDA_OUT();
	
    // ��ʼ
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
    // ��ȡSDA����Ȩ
    SDA_OUT();
    // 8��bit
    // �ȷ���λ
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
    // �����ӣ�send�����豸����pin���޸�pin��ƽ�ߵ�
    // �ӵ�����wait�����豸����pin���޸�pin��ƽ�ߵ�

    // SDA��ʼ��, ����
    SDA(1);
    DELAY();
	// SCL��ʼ��, ����
    SCL(1);
    DELAY();

    // ��SCL����Ϊ����ģʽ������SDA����Ȩ, �ȴ�������
    SDA_IN();
    DELAY();
		
    // ���SDA��ƽ�仯
//    if (SDA_STATE() == RESET) {
//        // ACK�ɹ�, �ָ�SCLΪ�͵�ƽ
//        SCL(0);
//		// ��ȡSDA����Ȩ
//		SDA_OUT();
//		DELAY();
//    } else {
//        // ACKʧ�ܣ�STOP
//        stop();
//		return 1;
//    }

	while(SDA_STATE() == SET && retry > 0){
		// ����
		retry--;
		DELAY();
	}
	
	if (retry <= 0){
		stop();
		return 1;
	}else {		
        // ACK�ɹ�, �ָ�SCLΪ�͵�ƽ
        SCL(0);
		// ��ȡSDA����Ȩ
		SDA_OUT();
	}
	
	return 0;

}


static void stop() {
    // ��ȡSDA����Ȩ
    SDA_OUT();

    // ����
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
    // I2C�����ʣ�100k 400k 1M 5M
    // ��(240M) ��(1M)
    // 100k��1000000us 100000bit, 10us 1bit, 5us
    // 400k��1000000us 400000bit, 10us 1bit, 2us

    // ���ã�Ӳʵ�֣���·ʵ��
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

    //////////// I2C��·
    rcu_periph_clock_enable(RCU_I2C0);

    i2c_deinit(i2cx);
    // I2C��������
    i2c_clock_config(i2cx, I2C0_SPEED, I2C_DTCY_2);

    // ʹ��i2c 0000 000r/w
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
    // addr�豸��ַ�� 0000 000(r/w)
    uint8_t waddr = addr << 1;
    // addr ��b1-b7
    /************* start ***********************/
    // �ȴ�I2C����
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    // start
    i2c_start_on_bus(i2cx);

    // �ȴ�I2C���豸�ɹ�������ʼ�ź�
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    /************* device address **************/
    // �����豸��ַ(д��ַ)
    i2c_master_addressing(i2cx, waddr, I2C_TRANSMITTER);
    // �ȴ���ַ�������
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************ register address ************/
    // �Ĵ�����ַ
    // �ȴ��������ݻ�����Ϊ��
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // ��������
    i2c_data_transmit(i2cx, reg);
    // �ȴ����ݷ������
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /***************** data ******************/
    // ��������
    uint32_t i;
    for(i = 0; i < len; i++) {
        uint8_t d = data[i];

        // �ȴ��������ݻ�����Ϊ��
        if(I2C_wait(I2C_FLAG_TBE)) return 6;

        // ��������
        i2c_data_transmit(i2cx, d);

        // �ȴ����ݷ������
        if(I2C_wait(I2C_FLAG_BTC)) return 7;
    }

    /***************** stop ********************/
    // stop
    i2c_stop_on_bus(i2cx);

    return I2C_RESULT_OK;// �ɹ�
}

uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len) {
    // addr�豸��ַ�� 0000 000(r/w)
    uint8_t waddr = addr << 1;
    // addr ��b1-b7
    /************* start ***********************/
    // �ȴ�I2C����
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    // start
    i2c_start_on_bus(i2cx);

    // �ȴ�I2C���豸�ɹ�������ʼ�ź�
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    /************* device address **************/
    // �����豸��ַ(д��ַ)
    i2c_master_addressing(i2cx, waddr, I2C_TRANSMITTER);
    // �ȴ���ַ�������
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************ register address ************/
    // �Ĵ�����ַ
    // �ȴ��������ݻ�����Ϊ��
    if(I2C_wait(I2C_FLAG_TBE)) return 4;
    // ��������
    i2c_data_transmit(i2cx, reg);
    // �ȴ����ݷ������
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /***************** data ******************/
    // ��������
    uint32_t i;
    for(i = 0; i < len; i++) {
        uint8_t d = data[i * offset];

        // �ȴ��������ݻ�����Ϊ��
        if(I2C_wait(I2C_FLAG_TBE)) return 6;

        // ��������
        i2c_data_transmit(i2cx, d);

        // �ȴ����ݷ������
        if(I2C_wait(I2C_FLAG_BTC)) return 7;
    }

    /***************** stop ********************/
    // stop
    i2c_stop_on_bus(i2cx);

    return I2C_RESULT_OK;// �ɹ�
}
uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len) {
    uint8_t waddr = addr << 1;
    uint8_t raddr = (addr << 1) | 1;
    // ��ʼ
    /************* start ***********************/
    // �ȴ�I2C����
    if(I2C_waitn(I2C_FLAG_I2CBSY)) return 1;

    // ���������ź�
    i2c_start_on_bus(i2cx);
    if(I2C_wait(I2C_FLAG_SBSEND)) return 2;

    // �����豸��ַ(д��ַ)
    /************* device address **************/
    // ���ʹ��豸��ַ
    i2c_master_addressing(i2cx, waddr, I2C_TRANSMITTER);

    if(I2C_wait(I2C_FLAG_ADDSEND)) return 3;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    // ���ͼĴ�����ַ
    /********** register address **************/
    // �ȴ����ͻ�����
    if(I2C_wait(I2C_FLAG_TBE)) return 4;

    // ���ͼĴ�����ַ
    i2c_data_transmit(i2cx, reg);

    // �ȴ������������
    if(I2C_wait(I2C_FLAG_BTC)) return 5;

    /************* start ***********************/
    // �����������ź�
    i2c_start_on_bus(i2cx);
    if(I2C_wait(I2C_FLAG_SBSEND)) return 7;


    /************* device address **************/
    // ���ʹ��豸��ַ
    i2c_master_addressing(i2cx, raddr, I2C_RECEIVER);
    if(I2C_wait(I2C_FLAG_ADDSEND)) return 8;
    i2c_flag_clear(i2cx, I2C_FLAG_ADDSEND);

    /************* data **************/
    //ack
    i2c_ack_config(i2cx, I2C_ACK_ENABLE);// send ack
    i2c_ackpos_config(i2cx, I2C_ACKPOS_CURRENT);// ����һ������֮�󣬾��Զ�����ack
    if(I2C_wait((I2C_CTL0(i2cx) & I2C_CTL0_ACKEN))) return 11;

    // ��ȡ����
    uint8_t i;
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            // �ڶ�ȡ���һ���ֽ�֮ǰ������ACK��������ֹͣ�ź�
            // �����Զ�NACK
            i2c_ack_config(i2cx, I2C_ACK_DISABLE);
			if(I2C_wait((I2C_CTL0(i2cx) & I2C_CTL0_ACKEN))) return 12;
        }
		
        // �ȴ�ACK���ݷ������
        // �ȴ����ջ�����
        if(I2C_wait(I2C_FLAG_RBNE)) return 10;
        data[i] = i2c_data_receive(i2cx);
    }
    /***************** stop ********************/
    i2c_stop_on_bus(i2cx);

    return 0;
}
#endif