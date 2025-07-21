#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include "systick.h"
#include "SPI.h"
#include "st7789.h"
#include "I2C.h"
#include "cst816t.h"

void example_st7789_test(){
	
	 
  ST7789_Init();
  ST7789_Test();
   
}
