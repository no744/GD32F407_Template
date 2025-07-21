#include "gd32f4xx.h"
#include <stdio.h>
#include "systick.h"
#include "I2C.h"
#include "i2c_oled.h"
#if 0
void example_i2c_oled_test(){
   
    I2C_init();
	  OLED_Init();
	
	u8 t=' ';
while(1) {
   // OLED_ShowPicture(0,0,128,64,BMP1,1);
    OLED_Refresh();
    delay_1ms(500);
    OLED_Clear();
    OLED_ShowChinese(0,0,0,16,1);//��
    OLED_ShowChinese(18,0,1,16,1);//��
    OLED_ShowChinese(36,0,2,16,1);//԰
    OLED_ShowChinese(54,0,3,16,1);//��
    OLED_ShowChinese(72,0,4,16,1);//��
    OLED_ShowChinese(90,0,5,16,1);//��
    OLED_ShowChinese(108,0,6,16,1);//��
    OLED_ShowString(8,16,"ZHONGJINGYUAN",16,1);
    OLED_ShowString(20,32,"2014/05/01",16,1);
    OLED_ShowString(0,48,"ASCII:",16,1);
    OLED_ShowString(63,48,"CODE:",16,1);
    OLED_ShowChar(48,48,t,16,1);//��ʾASCII�ַ�
    t++;
    if(t>'~')t=' ';
    OLED_ShowNum(103,48,t,3,16,1);
    OLED_Refresh();
    delay_1ms(500);
    OLED_Clear();
    OLED_ShowChinese(0,0,0,16,1);  //16*16 ��
    OLED_ShowChinese(16,0,0,24,1); //24*24 ��
    OLED_ShowChinese(24,20,0,32,1);//32*32 ��
    OLED_ShowChinese(64,0,0,64,1); //64*64 ��
    OLED_Refresh();
    delay_1ms(500);
    OLED_Clear();
    OLED_ShowString(0,0,"ABC",8,1);//6*8 ��ABC��
    OLED_ShowString(0,8,"ABC",12,1);//6*12 ��ABC��
    OLED_ShowString(0,20,"ABC",16,1);//8*16 ��ABC��
    OLED_ShowString(0,36,"ABC",24,1);//12*24 ��ABC��
    OLED_Refresh();
    delay_1ms(500);
    OLED_ScrollDisplay(11,4,1);
}
}
#endif