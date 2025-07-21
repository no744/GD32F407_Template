#ifndef __SPI0_H__
#define __SPI0_H__

#include "Spi_config.h"

void SPI0_init();

void SPI0_write(uint8_t data);

uint8_t SPI0_read();

uint8_t SPI0_read_write(uint8_t data);

#endif