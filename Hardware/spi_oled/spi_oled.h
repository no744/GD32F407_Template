#ifndef __SPI_OLED_H
#define __SPI_OLED_H 
#include "app_config.h"
#if USE_ICHEIMA_SPI_OLED

#include "gd32f4xx.h"
#include "SPI.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

//SCL=SCLK 
//SDA=MOSI
//DC=DC
//CS=CS1
//FS0=MOSI
//CS2=CS2
//-----------------OLED¶Ë¿Ú¶¨Òå---------------- 

#define SPI_WRITE(dat) 	SPI0_write(dat)
#define SPI_READ() 			SPI0_read()


#define	DC_RCU			RCU_GPIOA
#define	DC_PORT			GPIOA
#define DC_PIN			GPIO_PIN_2

#define	CS1_RCU			RCU_GPIOA
#define	CS1_PORT		GPIOA
#define CS1_PIN			GPIO_PIN_3


#define	CS2_RCU			RCU_GPIOC
#define	CS2_PORT		GPIOC
#define CS2_PIN			GPIO_PIN_5


//#define OLED_SCL_Clr() GPIO_ResetBits(SCL_PORT, SCL_PIN)//SCL
//#define OLED_SCL_Set() GPIO_SetBits(SCL_PORT, SCL_PIN)

//#define OLED_SDA_Clr() GPIO_ResetBits(MOSI_PORT,MOSI_PIN)//SDA
//#define OLED_SDA_Set() GPIO_SetBits(MOSI_PORT,MOSI_PIN)

//#define OLED_READ_FS0()    GPIO_ReadInputDataBit(MISO_PORT,MISO_PIN)//FS0

#define OLED_DC_Clr()  gpio_bit_reset(DC_PORT,DC_PIN)//DC
#define OLED_DC_Set()  gpio_bit_set(DC_PORT,DC_PIN)
 		     
#define OLED_CS_Clr()  gpio_bit_reset(CS1_PORT,CS1_PIN)//CS1
#define OLED_CS_Set()  gpio_bit_set(CS1_PORT,CS1_PIN)
 		     
#define OLED_ROM_CS_Clr()  gpio_bit_reset(CS2_PORT,CS2_PIN)//CS2
#define OLED_ROM_CS_Set()  gpio_bit_set(CS2_PORT,CS2_PIN)


void OLED_SPI_ColorTurn(u8 i);
void OLED_SPI_DisplayTurn(u8 i);
void OLED_SPI_Clear(void);
void OLED_SPI_address(u8 x,u8 y);
void OLED_SPI_Display_128x64(u8 *dp);
void OLED_SPI_Display_16x16(u8 x,u8 y,u8 *dp);
void OLED_SPI_Display_8x16(u8 x,u8 y,u8 *dp);
void OLED_SPI_Display_5x7(u8 x,u8 y,u8 *dp);
void OLED_SPI_get_data_from_ROM(u8 addrHigh,u8 addrMid,u8 addrLow,u8 *pbuff,u8 DataLen);
void OLED_SPI_Display_GB2312_string(u8 x,u8 y,char *text);
void OLED_SPI_Display_string_5x7(u8 x,u8 y,char *text);
void OLED_SPI_ShowNum(u8 x,u8 y,float num,u8 len);
void OLED_SPI_Init(void);
#endif


#endif
