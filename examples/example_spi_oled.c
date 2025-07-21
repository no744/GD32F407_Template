#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "SPI.h"
#include "spi_oled.h"


void example_spi_oled_test(){

    SPI_init();
	  OLED_SPI_Init();
	
	  OLED_SPI_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
    OLED_SPI_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
    while(1)
    {
        delay_1ms(500);
        OLED_SPI_Display_GB2312_string(0,0,"12864,�������ֿ�");	/*�ڵ�1ҳ����1�У���ʾһ��16x16�����ֻ�8x16��ASCII��*/
        OLED_SPI_Display_GB2312_string(0,2,"16X16���庺�ֿ�,");  /*��ʾһ��16x16�����ֻ�8x16��ASCII��.������ͬ*/
        OLED_SPI_Display_GB2312_string(0,4,"��8X16����ASCII,");
        OLED_SPI_Display_GB2312_string(0,6,"��5X7����ASCII��");
        delay_1ms(500);

    }
}