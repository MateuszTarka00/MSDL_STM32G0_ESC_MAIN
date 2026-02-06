/*
 * st7789_config.h
 *
 *  Created on: 6 lut 2026
 *      Author: mateo
 */

#ifndef ST7789_LCD_DRIVER_ST7789_CONFIG_H_
#define ST7789_LCD_DRIVER_ST7789_CONFIG_H_

/* choose display dimensions */
#define USING_320X240

/* choose display rotation */
#define ST7789_ROTATION 0

/* choose a Hardware SPI port to use. */
#define ST7789_SPI_PORT hspi2

/* choose whether use DMA or not */
#define USE_DMA 1

/* If u need CS control, comment below*/
//#define CFG_NO_CS
/* Pin connection*/
#define ST7789_RST_PORT LCD_RST_GPIO_Port
#define ST7789_RST_PIN  LCD_RST_Pin
#define ST7789_DC_PORT  LCD_DC_GPIO_Port
#define ST7789_DC_PIN   LCD_DC_Pin

#ifndef CFG_NO_CS
#define ST7789_CS_PORT  LCD_CS_GPIO_Port
#define ST7789_CS_PIN   LCD_CS_Pin
#endif

/* If u need Backlight control, uncomment below */
#define BLK_PORT LCD_BKL_GPIO_Port
#define BLK_PIN  LCD_BKL_Pin

#endif /* ST7789_LCD_DRIVER_ST7789_CONFIG_H_ */
