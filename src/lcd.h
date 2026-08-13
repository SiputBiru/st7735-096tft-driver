/*
 * lcd.h - ST7735 0.96" TFT drawing API + colors.
 * Ported from nologo factory firmware (stm32c8t6/HARDWARE/LCD/lcd.h / lcd.c).
 */
#ifndef LCD_H
#define LCD_H

#include "lcd_init.h"
#include "lcdfont.h"

#ifdef __cplusplus
extern "C" {
#endif

/* drawing primitives */
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);

/* text */
void LCD_ShowChinese(
  uint16_t x,
  uint16_t y,
  uint8_t* s,
  uint16_t fc,
  uint16_t bc,
  uint8_t sizey,
  uint8_t mode
);
void LCD_ShowChinese16x16(
  uint16_t x,
  uint16_t y,
  uint8_t* s,
  uint16_t fc,
  uint16_t bc,
  uint8_t sizey,
  uint8_t mode
);
void LCD_ShowChar(
  uint16_t x,
  uint16_t y,
  uint8_t num,
  uint16_t fc,
  uint16_t bc,
  uint8_t sizey,
  uint8_t mode
);
void LCD_ShowString(
  uint16_t x,
  uint16_t y,
  const char* p,
  uint16_t fc,
  uint16_t bc,
  uint8_t sizey,
  uint8_t mode
);
void LCD_ShowIntNum(
  uint16_t x,
  uint16_t y,
  uint16_t num,
  uint8_t len,
  uint16_t fc,
  uint16_t bc,
  uint8_t sizey
);
void LCD_ShowFloatNum1(
  uint16_t x,
  uint16_t y,
  float num,
  uint8_t len,
  uint16_t fc,
  uint16_t bc,
  uint8_t sizey
);

/* images: RGB565 byte pairs (hi, lo) */
void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[]);

#ifdef __cplusplus
}
#endif

/* colors (RGB565) */
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0xF81F
#define GRED 0xFFE0
#define GBLUE 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0xBC40
#define BRRED 0xFC07
#define GRAY 0x8430
#define DARKBLUE 0x01CF
#define LIGHTBLUE 0x7D7C
#define GRAYBLUE 0x5458
#define LIGHTGREEN 0x841F
#define LGRAY 0xC618
#define LGRAYBLUE 0xA651
#define LBBLUE 0x2B12

#endif /* LCD_H */
