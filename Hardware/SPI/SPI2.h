#ifndef __SPI2_H__
#define __SPI2_H__

#include "Spi_config.h"

void SPI2_init();

void SPI2_write(uint8_t data);

uint8_t SPI2_read();

uint8_t SPI2_read_write(uint8_t data);

#endif