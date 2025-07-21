#ifndef __I2C_CONFIG_H__
#define __I2C_CONFIG_H__

#include "gd32f4xx.h"
#include "systick.h"

#define USE_I2C0	1
#define USE_I2C1	0
#define USE_I2C2	0

#define I2C_RESULT_OK					0
#define I2C_ERR_DEVICE_ADDR		1
#define I2C_ERR_REGISTER			2
#define I2C_ERR_WRITE					3
#define I2C_ERR_READ					4

#if	USE_I2C0
//======================================================== I2C0

#define	I2C0_SOFT	  1

#define I2C0_SPEED 	400000

// SCL - PB6
// SDA - PB7
#define I2C0_SCL_RCU		RCU_GPIOB
#define I2C0_SCL_PORT		GPIOB
#define I2C0_SCL_PIN		GPIO_PIN_6

#define I2C0_SDA_RCU		RCU_GPIOB
#define I2C0_SDA_PORT		GPIOB
#define I2C0_SDA_PIN		GPIO_PIN_7
//========================================================
#endif


#endif