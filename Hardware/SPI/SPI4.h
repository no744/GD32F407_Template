#ifndef __SPI4_H__
#define __SPI4_H__

#include "Spi_config.h"

void SPI4_init();

void SPI4_write(uint8_t data);

uint8_t SPI4_read();

uint8_t SPI4_read_write(uint8_t data);

#endif