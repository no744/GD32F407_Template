#include "gd32f4xx.h"
#include "systick.h"
#include <stdio.h>
#include "usart0.h"
/*********************
任务目标:

模板工程

**********************/

int main(void) {
  
	// 系统滴答定时器初始化
	systick_config();
	usart_init();
// extern void example_i2c_oled_test();
// example_i2c_oled_test();
//  extern void example_u8g2_oled_test();
//	example_u8g2_oled_test();
  //extern	void example_spi_oled_test();
	//example_spi_oled_test();
	
	//extern  void example_st7789_test();
	//example_st7789_test();
	extern  void example_lvgl_test();
	example_lvgl_test();
	while(1) {
	  
	}
	
}