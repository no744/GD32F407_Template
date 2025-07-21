#ifndef __SPI_CONFIG_H__
#define __SPI_CONFIG_H__

#include "gd32f4xx.h"
#include "systick.h"

#define USE_SPI0	1
#define USE_SPI1	0
#define USE_SPI2	1
#define USE_SPI3	0
#define USE_SPI4	0
#define USE_SPI5	0

/******************************************SPI0************************************************/
#if USE_SPI0

#define SPI0_SOFT	0

// SCL
#define SPI0_SCL_RCU	RCU_GPIOA
#define SPI0_SCL_PORT	GPIOA
#define SPI0_SCL_PIN	GPIO_PIN_5
// MOSI
#define SPI0_MOSI_RCU	RCU_GPIOA
#define SPI0_MOSI_PORT	GPIOA
#define SPI0_MOSI_PIN	GPIO_PIN_7
// MISO
#define SPI0_MISO_RCU	RCU_GPIOA
#define SPI0_MISO_PORT	GPIOA
#define SPI0_MISO_PIN	GPIO_PIN_6

#if (SPI0_SOFT == 0)

#define SPI0_FRAME_SIZE          	SPI_FRAMESIZE_8BIT;		// default
#define SPI0_CLOCK_POLARITY_PHASE	SPI_CK_PL_HIGH_PH_2EDGE;// (1, 1)
#define SPI0_PRESCALE            	SPI_PSC_16;				// 分频, 120M/PSC 越大发的越慢
#define SPI0_ENDIAN              	SPI_ENDIAN_MSB;			// default

#endif

#endif



/******************************************SPI2************************************************/
#if USE_SPI2

#define SPI2_SOFT	0

// SCL
#define SPI2_SCL_RCU	RCU_GPIOC
#define SPI2_SCL_PORT	GPIOC
#define SPI2_SCL_PIN	GPIO_PIN_10
// MISO
#define SPI2_MISO_RCU	RCU_GPIOC
#define SPI2_MISO_PORT	GPIOC
#define SPI2_MISO_PIN	GPIO_PIN_11
// MOSI
#define SPI2_MOSI_RCU	RCU_GPIOD
#define SPI2_MOSI_PORT	GPIOD
#define SPI2_MOSI_PIN	GPIO_PIN_6


#if (SPI2_SOFT == 0)

#define SPI2_FRAME_SIZE          	SPI_FRAMESIZE_8BIT;		// default
#define SPI2_CLOCK_POLARITY_PHASE	SPI_CK_PL_LOW_PH_1EDGE;// (1, 1)
#define SPI2_PRESCALE            	SPI_PSC_16;				// 分频, 120M/PSC 越大发的越慢
#define SPI2_ENDIAN              	SPI_ENDIAN_MSB;			// default

#endif

#endif






/******************************************SPI4************************************************/
#if USE_SPI4

#define SPI4_SOFT	1

// SCL
#define SPI4_SCL_RCU	RCU_GPIOF
#define SPI4_SCL_PORT	GPIOF
#define SPI4_SCL_PIN	GPIO_PIN_7
// MOSI
#define SPI4_MOSI_RCU	RCU_GPIOF
#define SPI4_MOSI_PORT	GPIOF
#define SPI4_MOSI_PIN	GPIO_PIN_9
// MISO
#define SPI4_MISO_RCU	RCU_GPIOF
#define SPI4_MISO_PORT	GPIOF
#define SPI4_MISO_PIN	GPIO_PIN_8

#if (SPI4_SOFT == 0)

#define SPI4_FRAME_SIZE          	SPI_FRAMESIZE_8BIT;		// default
#define SPI4_CLOCK_POLARITY_PHASE	SPI_CK_PL_HIGH_PH_2EDGE;// (1, 1)
#define SPI4_PRESCALE            	SPI_PSC_16;				// 分频, 120M/PSC 越大发的越慢
#define SPI4_ENDIAN              	SPI_ENDIAN_MSB;			// default

#endif

#endif

#endif