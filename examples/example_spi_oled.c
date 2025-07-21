#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "SPI.h"
#include "spi_oled.h"


void example_spi_oled_test(){

    SPI_init();
	  OLED_SPI_Init();
	
	  OLED_SPI_ColorTurn(0);//0正常显示，1 反色显示
    OLED_SPI_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
    while(1)
    {
        delay_1ms(500);
        OLED_SPI_Display_GB2312_string(0,0,"12864,带中文字库");	/*在第1页，第1列，显示一串16x16点阵汉字或8x16的ASCII字*/
        OLED_SPI_Display_GB2312_string(0,2,"16X16简体汉字库,");  /*显示一串16x16点阵汉字或8x16的ASCII字.以下雷同*/
        OLED_SPI_Display_GB2312_string(0,4,"或8X16点阵ASCII,");
        OLED_SPI_Display_GB2312_string(0,6,"或5X7点阵ASCII码");
        delay_1ms(500);

    }
}