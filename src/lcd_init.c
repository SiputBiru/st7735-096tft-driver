/*
 * lcd_init.c - ST7735 0.96" TFT low-level driver (bit-banged SPI).
 * Direct port of nologo factory lcd_init.c, GPIO via raw register writes.
 *
 * Provenance: init sequence and structure derived from nologo factory
 * firmware (no explicit license). Personal/educational use. See README.
 */
#include "lcd_init.h"

/* GPIO: PA6, PA7, PB0, PB1, PB10, PB11 as push-pull output @ 50MHz */
void LCD_GPIO_Init(void) {
  /* Enable GPIOA (bit2) + GPIOB (bit3) clocks on APB2 */
  RCC->APB2ENR |= (1u << 2) | (1u << 3);

  /* PA6, PA7 (CRL nibbles 6, 7) */
  GPIOA->CRL &= ~(0xFu << 24);
  GPIOA->CRL |= (0x3u << 24); /* PA6: MODE=11 (50MHz), CNF=00 (push-pull) */
  GPIOA->CRL &= ~(0xFu << 28);
  GPIOA->CRL |= (0x3u << 28); /* PA7 */

  /* PB0, PB1 (CRL nibbles 0, 1) */
  GPIOB->CRL &= ~(0xFu << 0);
  GPIOB->CRL |= (0x3u << 0); /* PB0 */
  GPIOB->CRL &= ~(0xFu << 4);
  GPIOB->CRL |= (0x3u << 4); /* PB1 */

  /* PB10, PB11 (CRH nibbles 2, 3) */
  GPIOB->CRH &= ~(0xFu << 8);
  GPIOB->CRH |= (0x3u << 8); /* PB10 */
  GPIOB->CRH &= ~(0xFu << 12);
  GPIOB->CRH |= (0x3u << 12); /* PB11 */

  /* idle states */
  LCD_SCLK_Set();
  LCD_MOSI_Set();
  LCD_CS_Set();
  LCD_DC_Set();
  LCD_RES_Set();
  LCD_BLK_Set();
}

/* exact factory bit-bang (one byte, CS asserted around it) */
void LCD_Writ_Bus(uint8_t dat) {
  uint8_t i;
  LCD_CS_Clr();
  for (i = 0; i < 8; i++) {
    LCD_SCLK_Clr();
    if (dat & 0x80)
      LCD_MOSI_Set();
    else
      LCD_MOSI_Clr();
    LCD_SCLK_Set();
    dat <<= 1;
  }
  LCD_CS_Set();
}

void LCD_WR_DATA8(uint8_t dat) { LCD_Writ_Bus(dat); }

void LCD_WR_DATA(uint16_t dat) {
  LCD_Writ_Bus(dat >> 8);
  LCD_Writ_Bus(dat);
}

void LCD_WR_REG(uint8_t dat) {
  LCD_DC_Clr();
  LCD_Writ_Bus(dat);
  LCD_DC_Set();
}

/* Window + RAM write. Offsets depend on orientation (factory lcd_init.c):
 *   USE_HORIZONTAL 0/1 (portrait):  x+26, y+1
 *   USE_HORIZONTAL 2/3 (landscape): x+1,  y+26
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
#if (USE_HORIZONTAL == 0) || (USE_HORIZONTAL == 1)
  LCD_WR_REG(0x2a);
  LCD_WR_DATA(x1 + 26);
  LCD_WR_DATA(x2 + 26);
  LCD_WR_REG(0x2b);
  LCD_WR_DATA(y1 + 1);
  LCD_WR_DATA(y2 + 1);
  LCD_WR_REG(0x2c);
#else
  LCD_WR_REG(0x2a);
  LCD_WR_DATA(x1 + 1);
  LCD_WR_DATA(x2 + 1);
  LCD_WR_REG(0x2b);
  LCD_WR_DATA(y1 + 26);
  LCD_WR_DATA(y2 + 26);
  LCD_WR_REG(0x2c);
#endif
}

/* factory LCD_Init (register values identical to nologo lcd_init.c) */
void LCD_Init(void) {
  LCD_GPIO_Init();

  LCD_RES_Clr();
  LCD_DELAY_MS(100);
  LCD_RES_Set();
  LCD_DELAY_MS(100);
  LCD_BLK_Clr(); /* backlight ON */
  LCD_DELAY_MS(100);

  LCD_WR_REG(0x11); /* Sleep out */
  LCD_DELAY_MS(120);

  LCD_WR_REG(0xB1);
  LCD_WR_DATA8(0x05);
  LCD_WR_DATA8(0x3C);
  LCD_WR_DATA8(0x3C);
  LCD_WR_REG(0xB2);
  LCD_WR_DATA8(0x05);
  LCD_WR_DATA8(0x3C);
  LCD_WR_DATA8(0x3C);
  LCD_WR_REG(0xB3);
  LCD_WR_DATA8(0x05);
  LCD_WR_DATA8(0x3C);
  LCD_WR_DATA8(0x3C);
  LCD_WR_DATA8(0x05);
  LCD_WR_DATA8(0x3C);
  LCD_WR_DATA8(0x3C);
  LCD_WR_REG(0xB4);
  LCD_WR_DATA8(0x03);

  LCD_WR_REG(0xC0);
  LCD_WR_DATA8(0xAB);
  LCD_WR_DATA8(0x0B);
  LCD_WR_DATA8(0x04);
  LCD_WR_REG(0xC1);
  LCD_WR_DATA8(0xC5);
  LCD_WR_REG(0xC2);
  LCD_WR_DATA8(0x0D);
  LCD_WR_DATA8(0x00);
  LCD_WR_REG(0xC3);
  LCD_WR_DATA8(0x8D);
  LCD_WR_DATA8(0x6A);
  LCD_WR_REG(0xC4);
  LCD_WR_DATA8(0x8D);
  LCD_WR_DATA8(0xEE);
  LCD_WR_REG(0xC5);
  LCD_WR_DATA8(0x0F);

  LCD_WR_REG(0xE0);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x0E);
  LCD_WR_DATA8(0x08);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x10);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x02);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x09);
  LCD_WR_DATA8(0x0F);
  LCD_WR_DATA8(0x25);
  LCD_WR_DATA8(0x36);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x08);
  LCD_WR_DATA8(0x04);
  LCD_WR_DATA8(0x10);
  LCD_WR_REG(0xE1);
  LCD_WR_DATA8(0x0A);
  LCD_WR_DATA8(0x0D);
  LCD_WR_DATA8(0x08);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x0F);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x02);
  LCD_WR_DATA8(0x07);
  LCD_WR_DATA8(0x09);
  LCD_WR_DATA8(0x0F);
  LCD_WR_DATA8(0x25);
  LCD_WR_DATA8(0x35);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x09);
  LCD_WR_DATA8(0x04);
  LCD_WR_DATA8(0x10);

  LCD_WR_REG(0xFC);
  LCD_WR_DATA8(0x80);
  LCD_WR_REG(0x3A);
  LCD_WR_DATA8(0x05); /* RGB565 */

#if (USE_HORIZONTAL == 0)
  LCD_WR_REG(0x36);
  LCD_WR_DATA8(0x08);
#elif (USE_HORIZONTAL == 1)
  LCD_WR_REG(0x36);
  LCD_WR_DATA8(0xC8);
#elif (USE_HORIZONTAL == 2)
  LCD_WR_REG(0x36);
  LCD_WR_DATA8(0x78);
#else
  LCD_WR_REG(0x36);
  LCD_WR_DATA8(0xA8);
#endif

  LCD_WR_REG(0x21); /* Display inversion ON (IPS) */
  LCD_WR_REG(0x29); /* Display ON */

  LCD_WR_REG(0x2A);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x1A);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x69);
  LCD_WR_REG(0x2B);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0x01);
  LCD_WR_DATA8(0x00);
  LCD_WR_DATA8(0xA0);
  LCD_WR_REG(0x2C);
}
