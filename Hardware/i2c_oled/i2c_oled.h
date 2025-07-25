#ifndef __I2C_OLED_H
#define __I2C_OLED_H 
#include "app_config.h"
#include "gd32f4xx.h"
#include "stdlib.h"	
#include "I2C.h"	
#if USE_ICHEIMA_I2C_OLED
#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

//-----------------OLED端口定义---------------- 

//#define OLED_SCL_Clr() gpio_bit_reset(GPIOB,GPIO_PIN_6)//SCL
//#define OLED_SCL_Set() gpio_bit_set(GPIOB,GPIO_PIN_6)

//#define OLED_SDA_Clr() gpio_bit_reset(GPIOB,GPIO_PIN_7)//DIN
//#define OLED_SDA_Set() gpio_bit_set(GPIOB,GPIO_PIN_7)

//#define OLED_RES_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_4)//RES
//#define OLED_RES_Set() GPIO_SetBits(GPIOD,GPIO_Pin_4)

#define I2C_OLED_WRITE(addr, reg, data, len)				  I2C0_write(addr, reg, data, len)
#define I2C_OLED_WRITE2(addr, reg, data, offset, len)	I2C0_write2(addr, reg, data, offset, len)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
//void I2C_Start(void);
//void I2C_Stop(void);
//void I2C_WaitAck(void);
//void Send_Byte(u8 dat);

void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowString(u8 x,u8 y,char *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
void OLED_Init(void);

#endif

#endif