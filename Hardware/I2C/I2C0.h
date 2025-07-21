#ifndef __I2C0_H__
#define __I2C0_H__

#include "I2C_config.h"

void I2C0_init();

uint8_t I2C0_write(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);
uint8_t I2C0_write2(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t offset, uint32_t len);
uint8_t I2C0_read(uint8_t addr, uint8_t reg, uint8_t* data, uint32_t len);

#endif
