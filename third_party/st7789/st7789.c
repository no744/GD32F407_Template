#include "st7789.h"
#include "systick.h"

#include <stdio.h>

#define SPI_CLK_RCU RCU_GPIOA
#define SPI_CLK_PORT GPIOA
#define SPI_CLK_PIN GPIO_PIN_5
#define SPI_CLK_AF GPIO_AF_5

#define SPI_SDA_RCU RCU_GPIOA
#define SPI_SDA_PORT GPIOA
#define SPI_SDA_PIN GPIO_PIN_7
#define SPI_SDA_AF GPIO_AF_5


void ST7789_SPI_Init(){
	// PC10 SCL
	rcu_periph_clock_enable(SPI_CLK_RCU);
	gpio_mode_set(SPI_CLK_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI_CLK_PIN);
	gpio_af_set(SPI_CLK_PORT, SPI_CLK_AF, SPI_CLK_PIN);
  gpio_output_options_set(SPI_CLK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI_CLK_PIN);

		// PC12  SDA
	rcu_periph_clock_enable(SPI_SDA_RCU);
	gpio_mode_set(SPI_SDA_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SPI_SDA_PIN);
	gpio_af_set(SPI_SDA_PORT, SPI_SDA_AF, SPI_SDA_PIN);
  gpio_output_options_set(SPI_SDA_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, SPI_SDA_PIN);

	
	rcu_periph_clock_enable(ST7789_SPIX_RCU);
	spi_parameter_struct spi_init_struct;
	spi_i2s_deinit(ST7789_SPIX_PORT);
	 /* SPI2 parameter config */
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.prescale             = SPI_PSC_2;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	
	spi_init(ST7789_SPIX_PORT, &spi_init_struct);
	spi_enable(ST7789_SPIX_PORT);	
}

void ST7789_SPI_Write(uint8_t dat){		
		
		/* loop while spi tx data register in not emplty */
    while (RESET == spi_i2s_flag_get(ST7789_SPIX_PORT, SPI_FLAG_TBE)) ;
		/* send byte through the SPI2 peripheral */
    spi_i2s_data_transmit(ST7789_SPIX_PORT, dat);
    while (RESET == spi_i2s_flag_get(ST7789_SPIX_PORT, SPI_FLAG_RBNE)) ;
		spi_i2s_data_receive(ST7789_SPIX_PORT);
		
}

/**
 * @brief Write command to ST7789 controller
 * @param cmd -> command to write
 * @return none
 */
static void ST7789_WriteCommand(uint8_t cmd)
{
	ST7789_Select();
	ST7789_DC_Clr();
	ST7789_SPI_Write(cmd); 
	ST7789_UnSelect();
}




/**
 * @brief Write data to ST7789 controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
static void ST7789_WriteData(uint8_t *buff, size_t buff_size)
{
	ST7789_Select();
	ST7789_DC_Set();

	// split data in small chunks because HAL can't send more than 64K at once
	 /* while there is data to be written on the flash */
    while(buff_size > 0) {
        /* send the current byte */
				//printf("%x\r\n",*buff);
        ST7789_SPI_Write(*buff);
        /* point on the next byte to be written */
        buff+=1;
				buff_size--;
    }

	ST7789_UnSelect();
}
/**
 * @brief Write data to ST7789 controller, simplify for 8bit data.
 * data -> data to write
 * @return none
 */
static void ST7789_WriteSmallData(uint8_t data)
{
	ST7789_Select();
	ST7789_DC_Set();
	ST7789_SPI_Write( data); 
	ST7789_UnSelect();
}

/**
 * @brief Set the rotation direction of the display
 * @param m -> rotation parameter(please refer it in st7789.h)
 * @return none
 */
void ST7789_SetRotation(uint8_t m)
{
	ST7789_WriteCommand(ST7789_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		break;
	case 1:
		ST7789_WriteSmallData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	case 2:
		ST7789_WriteSmallData(ST7789_MADCTL_RGB);
		break;
	case 3:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	default:
		break;
	}
}

/**
 * @brief Set address of DisplayWindow
 * @param xi&yi -> coordinates of window
 * @return none
 */
static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	ST7789_Select();
	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;
	
	
	/* Column Address set*/
	ST7789_WriteCommand(ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start , x_end >> 8, x_end };
		ST7789_WriteData(data, sizeof(data));
	} 

	/* Row Address set */
	ST7789_WriteCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start , y_end >> 8, y_end };
		ST7789_WriteData(data, sizeof(data));
	}
	
	/* Column Address set
	ST7789_WriteCommand(ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start , x_end >> 8, x_end };
		ST7789_WriteData(data, sizeof(data));
	} */

	/* Row Address set 
	ST7789_WriteCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		ST7789_WriteData(data, sizeof(data));
	}*/
	/* Write to RAM */
	ST7789_WriteCommand(ST7789_RAMWR);
	ST7789_UnSelect();
}

/**
 * @brief Initialize ST7789 controller
 * @param none
 * @return none
 */
void ST7789_Init(void)
{
		ST7789_SPI_Init();
		// RST  PD6
		rcu_periph_clock_enable(ST7789_RST_RCU);
		gpio_mode_set(ST7789_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ST7789_RST_PIN);
		gpio_output_options_set(ST7789_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, ST7789_RST_PIN);
		gpio_bit_set(ST7789_RST_PORT, ST7789_RST_PIN);
		// DC  PB1
		rcu_periph_clock_enable(ST7789_DC_RCU);
		gpio_mode_set(ST7789_DC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ST7789_DC_PIN);
		gpio_output_options_set(ST7789_DC_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, ST7789_DC_PIN);
		gpio_bit_set(ST7789_DC_PORT, ST7789_DC_PIN);
		// CS PB0
		rcu_periph_clock_enable(ST7789_CS_RCU);
		gpio_mode_set(ST7789_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ST7789_CS_PIN);
		gpio_output_options_set(ST7789_CS_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, ST7789_CS_PIN);
		gpio_bit_set(ST7789_CS_PORT, ST7789_CS_PIN);
		// BLK PB10
	  rcu_periph_clock_enable(ST7789_BLK_RCU);
		gpio_mode_set(ST7789_BLK_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ST7789_BLK_PIN);
		gpio_output_options_set(ST7789_BLK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, ST7789_BLK_PIN);
		gpio_bit_set(ST7789_BLK_PORT, ST7789_BLK_PIN);
		
		delay_1ms(25);
    ST7789_RST_Clr();
    delay_1ms(25);
    ST7789_RST_Set();
    delay_1ms(50);
		ST7789_BLK_Set();
		ST7789_WriteCommand(0x11);
		//ST7789_SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)
		
		ST7789_WriteCommand(0x36);
		ST7789_WriteSmallData(0x00);
		
    ST7789_WriteCommand(0x3A);		//	Set color mode
    ST7789_WriteSmallData(0x65);
  	ST7789_WriteCommand(0xB2);				//	Porch control
	{
		uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
		ST7789_WriteData(data, sizeof(data));
	}
	
	
	/* Internal LCD Voltage generator settings */
    ST7789_WriteCommand(0XB7);				//	Gate Control
    ST7789_WriteSmallData(0x72);			//	Default value
    ST7789_WriteCommand(0xBB);				//	VCOM setting
    ST7789_WriteSmallData(0x3d);			//	0.725v (default 0.75v for 0x20)
    ST7789_WriteCommand(0xC0);				//	LCMCTRL	
    ST7789_WriteSmallData (0x2C);			//	Default value
    ST7789_WriteCommand (0xC2);				//	VDV and VRH command Enable
    ST7789_WriteSmallData (0x01);			//	Default value
    ST7789_WriteCommand (0xC3);				//	VRH set
    ST7789_WriteSmallData (0x19);			//	+-4.45v (defalut +-4.1v for 0x0B)
    ST7789_WriteCommand (0xC4);				//	VDV set
    ST7789_WriteSmallData (0x20);			//	Default value
    ST7789_WriteCommand (0xC6);				//	Frame rate control in normal mode
    ST7789_WriteSmallData (0x01);			//	Default value (60HZ)
    ST7789_WriteCommand (0xD0);				//	Power control
    ST7789_WriteSmallData (0xA4);			//	Default value
    ST7789_WriteSmallData (0xA1);			//	Default value
	/**************** Division line ****************/

	ST7789_WriteCommand(0xE0);
	{
		uint8_t data[] = {0xD0, 0x08, 0x0E, 0x09, 0x09, 0x05, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34};
		ST7789_WriteData(data, sizeof(data));
	}

    ST7789_WriteCommand(0xE1);
	{
		uint8_t data[] = {0xD0, 0x08, 0x0E, 0x09, 0x09, 0x15, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34};
		ST7789_WriteData(data, sizeof(data));
	}
    ST7789_WriteCommand (ST7789_INVON);		//	Inversion ON
		ST7789_WriteCommand (ST7789_SLPOUT);	//	Out of sleep mode
  	ST7789_WriteCommand (ST7789_NORON);		//	Normal Display on
  	ST7789_WriteCommand (ST7789_DISPON);	//	Main screen turned on	

	//ST7789_TearEffect(1);
	delay_1ms(50);
	ST7789_Fill_Color(RED);				//	Fill with Black.
}

/**
 * @brief Fill the DisplayWindow with single color
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill_Color(uint16_t color)
{
	uint16_t i, j;
	ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	ST7789_Select();
	for (i = 0; i < ST7789_WIDTH; i++)
		for (j = 0; j < ST7789_HEIGHT; j++) {
			uint8_t data[] = {color >> 8, color & 0xFF};
			//ST7789_WriteData(data, sizeof(data));
			ST7789_WriteSmallData(color>>8);
			ST7789_WriteSmallData(color&0xFF);
		}
	ST7789_UnSelect();
}

/**
 * @brief Draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x < 0) || (x >= ST7789_WIDTH) ||
		 (y < 0) || (y >= ST7789_HEIGHT))	return;
	
	ST7789_SetAddressWindow(x, y, x, y);
	uint8_t data[] = {color >> 8, color & 0xFF};
	ST7789_Select();
	ST7789_WriteData(data, sizeof(data));
	ST7789_UnSelect();
}

/**
 * @brief Fill an Area with single color
 * @param xSta&ySta -> coordinate of the start point
 * @param xEnd&yEnd -> coordinate of the end point
 * @param color -> color to Fill with
 * @return none
 */
void ST7789_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t* color)
{
	//printf("%d %d %d %d %X\r\n",xSta,ySta,xEnd,yEnd,color);
	if ((xEnd < 0) || (xEnd >= ST7789_WIDTH) ||
		 (yEnd < 0) || (yEnd >= ST7789_HEIGHT))	return;
	ST7789_Select();
	uint16_t i, j;
	ST7789_SetAddressWindow(xSta, ySta, xEnd, yEnd);
	
	
	uint32_t size = 0;
	size = ((xEnd - xSta) + 1) * ((yEnd - ySta) + 1)*2;
	
	
	ST7789_WriteData((uint8_t *)color, size);
	/*
	for (i = ySta; i <= yEnd; i++){
		for (j = xSta; j <= xEnd; j++) {
			uint8_t data[] = {color >> 8, color};
			//ST7789_WriteData(data, sizeof(data));
			ST7789_WriteSmallData(color >> 8);
			ST7789_WriteSmallData(color);
		}
	}*/
	
	ST7789_UnSelect();
}

/**
 * @brief Draw a big Pixel at a point
 * @param x&y -> coordinate of the point
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x <= 0) || (x > ST7789_WIDTH) ||
		 (y <= 0) || (y > ST7789_HEIGHT))	return;
	ST7789_Select();
	ST7789_Fill(x - 1, y - 1, x + 1, y + 1, &color);
	ST7789_UnSelect();
}

/**
 * @brief Draw a line with single color
 * @param x1&y1 -> coordinate of the start point
 * @param x2&y2 -> coordinate of the end point
 * @param color -> color of the line to Draw
 * @return none
 */
void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
        uint16_t color) {
	uint16_t swap;
    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
        //_swap_int16_t(x0, y0);
        //_swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
        //_swap_int16_t(x0, x1);
        //_swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            ST7789_DrawPixel(y0, x0, color);
        } else {
            ST7789_DrawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/**
 * @brief Draw a Rectangle with single color
 * @param xi&yi -> 2 coordinates of 2 top points.
 * @param color -> color of the Rectangle line
 * @return none
 */
void ST7789_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	ST7789_Select();
	ST7789_DrawLine(x1, y1, x2, y1, color);
	ST7789_DrawLine(x1, y1, x1, y2, color);
	ST7789_DrawLine(x1, y2, x2, y2, color);
	ST7789_DrawLine(x2, y1, x2, y2, color);
	ST7789_UnSelect();
}

/** 
 * @brief Draw a circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle line
 * @return  none
 */
void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_Select();
	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawPixel(x0 + x, y0 + y, color);
		ST7789_DrawPixel(x0 - x, y0 + y, color);
		ST7789_DrawPixel(x0 + x, y0 - y, color);
		ST7789_DrawPixel(x0 - x, y0 - y, color);

		ST7789_DrawPixel(x0 + y, y0 + x, color);
		ST7789_DrawPixel(x0 - y, y0 + x, color);
		ST7789_DrawPixel(x0 + y, y0 - x, color);
		ST7789_DrawPixel(x0 - y, y0 - x, color);
	}
	ST7789_UnSelect();
}

/**
 * @brief Draw an Image on the screen
 * @param x&y -> start point of the Image
 * @param w&h -> width & height of the Image to Draw
 * @param data -> pointer of the Image array
 * @return none
 */
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
	if ((x >= ST7789_WIDTH) || (y >= ST7789_HEIGHT))
		return;
	if ((x + w - 1) >= ST7789_WIDTH)
		return;
	if ((y + h - 1) >= ST7789_HEIGHT)
		return;

	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
	ST7789_WriteData((uint8_t *)data, sizeof(uint16_t) * w * h);
	ST7789_UnSelect();
}

/**
 * @brief Invert Fullscreen color
 * @param invert -> Whether to invert
 * @return none
 */
void ST7789_InvertColors(uint8_t invert)
{
	ST7789_Select();
	ST7789_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
	ST7789_UnSelect();
}

/** 
 * @brief Write a char
 * @param  x&y -> cursor of the start point.
 * @param ch -> char to write
 * @param font -> fontstyle of the string
 * @param color -> color of the char
 * @param bgcolor -> background color of the char
 * @return  none

void ST7789_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;
	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) {
			if ((b << j) & 0x8000) {
				uint8_t data[] = {color >> 8, color & 0xFF};
				ST7789_WriteData(data, sizeof(data));
			}
			else {
				uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
				ST7789_WriteData(data, sizeof(data));
			}
		}
	}
	ST7789_UnSelect();
} */

/** 
 * @brief Write a string 
 * @param  x&y -> cursor of the start point.
 * @param str -> string to write
 * @param font -> fontstyle of the string
 * @param color -> color of the string
 * @param bgcolor -> background color of the string
 * @return  none
 
void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	ST7789_Select();
	while (*str) {
		if (x + font.width >= ST7789_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		ST7789_WriteChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
	ST7789_UnSelect();
}*/

/** 
 * @brief Draw a filled Rectangle with single color
 * @param  x&y -> coordinates of the starting point
 * @param w&h -> width & height of the Rectangle
 * @param color -> color of the Rectangle
 * @return  none
 */
void ST7789_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	ST7789_Select();
	uint8_t i;

	/* Check input parameters */
	if (x >= ST7789_WIDTH ||
		y >= ST7789_HEIGHT) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= ST7789_WIDTH) {
		w = ST7789_WIDTH - x;
	}
	if ((y + h) >= ST7789_HEIGHT) {
		h = ST7789_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		ST7789_DrawLine(x, y + i, x + w, y + i, color);
	}
	ST7789_UnSelect();
}

/** 
 * @brief Draw a Triangle with single color
 * @param  xi&yi -> 3 coordinates of 3 top points.
 * @param color ->color of the lines
 * @return  none
 */
void ST7789_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
	ST7789_Select();
	/* Draw lines */
	ST7789_DrawLine(x1, y1, x2, y2, color);
	ST7789_DrawLine(x2, y2, x3, y3, color);
	ST7789_DrawLine(x3, y3, x1, y1, color);
	ST7789_UnSelect();
}

/** 
 * @brief Draw a filled Triangle with single color
 * @param  xi&yi -> 3 coordinates of 3 top points.
 * @param color ->color of the triangle
 * @return  none
 */
void ST7789_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
	ST7789_Select();
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	}
	else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	}
	else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7789_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
	ST7789_UnSelect();
}

/** 
 * @brief Draw a Filled circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle
 * @return  none
 */
void ST7789_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	ST7789_Select();
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);
	ST7789_DrawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		ST7789_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		ST7789_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		ST7789_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
	ST7789_UnSelect();
}


/**
 * @brief Open/Close tearing effect line
 * @param tear -> Whether to tear
 * @return none
 */
void ST7789_TearEffect(uint8_t tear)
{
	ST7789_Select();
	ST7789_WriteCommand(tear ? 0x35 /* TEON */ : 0x34 /* TEOFF */);
	ST7789_UnSelect();
}


/** 
 * @brief A Simple test function for ST7789
 * @param  none
 * @return  none
 */
void ST7789_Test(void)
{
	ST7789_Fill_Color(WHITE);
	printf("ST7789_Test\r\n");
	delay_1ms(500);
	ST7789_Fill_Color(CYAN);
    delay_1ms(500);
	ST7789_Fill_Color(RED);
    delay_1ms(500);
	ST7789_Fill_Color(BLUE);
    delay_1ms(500);
	ST7789_Fill_Color(GREEN);
    delay_1ms(500);
	ST7789_Fill_Color(YELLOW);
    delay_1ms(500);
	ST7789_Fill_Color(BROWN);
    delay_1ms(500);
	ST7789_Fill_Color(DARKBLUE);
    delay_1ms(500);
	ST7789_Fill_Color(MAGENTA);
    delay_1ms(500);
	ST7789_Fill_Color(LIGHTGREEN);
    delay_1ms(500);
	ST7789_Fill_Color(LGRAY);
    delay_1ms(500);
	ST7789_Fill_Color(LBBLUE);
    delay_1ms(500);
	ST7789_Fill_Color(WHITE);
	delay_1ms(500);

	/*ST7789_WriteString(10, 10, "Font test.", Font_16x26, GBLUE, WHITE);
	ST7789_WriteString(10, 50, "Hello Steve!", Font_7x10, RED, WHITE);
	ST7789_WriteString(10, 75, "Hello Steve!", Font_11x18, YELLOW, WHITE);
	ST7789_WriteString(10, 100, "Hello Steve!", Font_16x26, MAGENTA, WHITE);
	delay_1ms(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Rect./Line.", Font_11x18, YELLOW, RED);
	ST7789_DrawRectangle(30, 30, 100, 100, WHITE);
	delay_1ms(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Filled Rect.", Font_11x18, YELLOW, RED);
	ST7789_DrawFilledRectangle(30, 30, 50, 50, WHITE);
	delay_1ms(1000);


	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Circle.", Font_11x18, YELLOW, RED);
	ST7789_DrawCircle(60, 60, 25, WHITE);
	delay_1ms(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Filled Cir.", Font_11x18, YELLOW, RED);
	ST7789_DrawFilledCircle(60, 60, 25, WHITE);
	delay_1ms(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Triangle", Font_11x18, YELLOW, RED);
	ST7789_DrawTriangle(30, 30, 30, 70, 60, 40, WHITE);
	delay_1ms(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Filled Tri", Font_11x18, YELLOW, RED);
	ST7789_DrawFilledTriangle(30, 30, 30, 70, 60, 40, WHITE);
	delay_1ms(1000);*/

	//	If FLASH cannot storage anymore datas, please delete codes below.
	ST7789_Fill_Color(WHITE);
//	ST7789_DrawImage(0, 0, 128, 128, (uint16_t *)saber);
	delay_1ms(500);
}




























