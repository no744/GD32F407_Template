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
#include "ui.h"
void example8_button(){
  //��ȡ��ǰ��Ļ
  lv_obj_t* screen = lv_scr_act();
  //����Ҫ��ʾ���󣺰�ť
  lv_obj_t* btn = lv_btn_create(screen);
	lv_obj_set_style_bg_color(screen,lv_palette_main(LV_PALETTE_RED),0);
	lv_obj_add_flag(btn,LV_OBJ_FLAG_CHECKABLE);
  //����ʾ��������
  lv_obj_set_size(btn,100,60);

  //����ť����ı�
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
	lv_port_indev_init();
	//example8_button();
	ui_init();
	LV_IMG_DECLARE(astro);
  //����gifui_panel2
  lv_obj_t* gif = lv_gif_create(ui_Panel2);
  //����
  lv_gif_set_src(gif,&astro);
  while(1){
	   lv_tick_inc(5);
		 lv_timer_handler();
		 delay_1ms(5);
	}
}