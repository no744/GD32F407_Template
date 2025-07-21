#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "SPI.h"
#include "st7789.h"
#include "I2C.h"
#include "cst816t.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

void example8_button(){
  //获取当前屏幕
  lv_obj_t* screen = lv_scr_act();
  //创建要显示对象：按钮
  lv_obj_t* btn = lv_btn_create(screen);
  //对显示进行设置
  lv_obj_set_size(btn,100,60);

  //给按钮添加文本
  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label,"hello");
  lv_obj_align(label,LV_ALIGN_CENTER,0,0);
  lv_obj_align(btn,LV_ALIGN_CENTER,0,0);
}
void example_lvgl_test(){
   
	lv_init();
	
	I2C_init();
	//ST7789_Init();
	//CST816T_Init();
	lv_port_disp_init();
	example8_button();
  while(1){
	   lv_tick_inc(5);
		 lv_timer_handler();
		 delay_1ms(5);
	}
}