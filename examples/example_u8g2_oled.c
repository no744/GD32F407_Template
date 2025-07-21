#include "u8g2.h"
#include "gd32f4xx.h"
#include <stdio.h>
#include "systick.h"
#include <string.h>


uint8_t u8g2_gpio_and_delay_gd32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg){
		//Initialize SPI peripheral
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			/* HAL initialization contains all what we need so we can skip this part. */

		break;

		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_MILLI:
		 delay_1ms(arg_int);

		break;
		//Function which delays 10us
		case U8X8_MSG_DELAY_10MICRO:
		delay_1us(10);

		break;
		//Function which delays 100ns
		case U8X8_MSG_DELAY_100NANO:
		__NOP();

		break;
		//Function to define the logic level of the clockline
		case U8X8_MSG_GPIO_I2C_CLOCK:
			if (arg_int) gpio_bit_write(GPIOB, GPIO_PIN_6, SET);
			else gpio_bit_write(GPIOB, GPIO_PIN_6, RESET);

		break;
		//Function to define the logic level of the data line to the display
		case U8X8_MSG_GPIO_I2C_DATA:
			if (arg_int) gpio_bit_write(GPIOB, GPIO_PIN_7, SET);
			else gpio_bit_write(GPIOB, GPIO_PIN_7, RESET);

		break;
		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}

void example_u8g2_oled_test(){
	
	rcu_periph_clock_enable(RCU_GPIOB);
	
	gpio_mode_set(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_PIN_6);
	
	gpio_output_options_set(GPIOB,GPIO_OTYPE_OD,GPIO_OSPEED_MAX,GPIO_PIN_6);
	
  gpio_mode_set(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_PIN_7);
	
	gpio_output_options_set(GPIOB,GPIO_OTYPE_OD,GPIO_OSPEED_MAX,GPIO_PIN_7);
  /* USER CODE BEGIN 1 */
	u8g2_t u8g2;
  
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_gd32);
  u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
  u8g2_SetPowerSave(&u8g2, 0); // wake up display


  

    
	

   	u8g2_ClearBuffer(&u8g2);  // 清空缓冲区
    u8g2_SetFontMode(&u8g2, 1);  
    u8g2_SetFontDirection(&u8g2, 0); // 设置方向
    u8g2_SetFont(&u8g2, u8g2_font_fub20_tf); // 设置字体
    u8g2_DrawStr(&u8g2, 0, 28, "ICHEIMA");  // 设置x,y坐标及字体内容
    
    u8g2_DrawHLine(&u8g2, 2, 35, 47); // 水平线起点，x,y，长度
    u8g2_DrawHLine(&u8g2, 3, 36, 47);
    u8g2_DrawVLine(&u8g2, 45, 32, 12);
    u8g2_DrawVLine(&u8g2, 46, 33, 12);
  
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(&u8g2, 1,54,"www.icheima.com");
	
	  u8g2_SendBuffer(&u8g2); // 上面步骤画完，一定要记得将数据发送到屏幕
	  delay_1ms(1000);
}







