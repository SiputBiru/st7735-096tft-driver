/*
 * lcd_init.h - ST7735 0.96" TFT low-level driver (bit-banged SPI).
 *
 * Ported verbatim from nologo's factory firmware (stm32c8t6 factory test)
 * but using direct GPIO register writes instead of the SPL.
 *
 * Why direct registers (BSRR/BRR at 50MHz push-pull) and not the usual
 * Arduino/Adafruit software-SPI? Because on the STM32duino core, pinMode()
 * defaults to a 2MHz drive and the resulting clock edges were too soft for
 * the ST7735 to latch. The factory firmware drives the pins as 50MHz
 * push-pull with one-instruction toggles, and that is what actually works.
 *
 * Pinout (factory-verified, nologo STM32F103C8T6 0.96" TFT board):
 *   SCK = PB10   MOSI = PB11   RES = PA7   DC = PB0   CS = PB1   BLK = PA6
 *
 * To use different pins, edit the macros below (register bit-bang, so you
 * need BSRR/BRR-capable ports; swapping to another STM32F1 pin is trivial).
 */
#ifndef LCD_INIT_H
#define LCD_INIT_H

#include "board.h"
#include <stdint.h>

/* 0/1 = portrait 80x160, 2/3 = landscape 160x80 (factory default = 2) */
#define USE_HORIZONTAL 2

#if (USE_HORIZONTAL == 0) || (USE_HORIZONTAL == 1)
#define LCD_W 80
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 80
#endif

/* Pin macros: direct BSRR/BRR register access (50MHz push-pull) */
#define LCD_SCLK_Clr() (GPIOB->BRR = (1u << 10))
#define LCD_SCLK_Set() (GPIOB->BSRR = (1u << 10))
#define LCD_MOSI_Clr() (GPIOB->BRR = (1u << 11))
#define LCD_MOSI_Set() (GPIOB->BSRR = (1u << 11))
#define LCD_RES_Clr() (GPIOA->BRR = (1u << 7))
#define LCD_RES_Set() (GPIOA->BSRR = (1u << 7))
#define LCD_DC_Clr() (GPIOB->BRR = (1u << 0))
#define LCD_DC_Set() (GPIOB->BSRR = (1u << 0))
#define LCD_CS_Clr() (GPIOB->BRR = (1u << 1))
#define LCD_CS_Set() (GPIOB->BSRR = (1u << 1))
#define LCD_BLK_Clr() (GPIOA->BRR = (1u << 6))
#define LCD_BLK_Set() (GPIOA->BSRR = (1u << 6))

#ifdef __cplusplus
extern "C" {
#endif

void LCD_GPIO_Init(void);
void LCD_Writ_Bus(uint8_t dat);
void LCD_WR_DATA8(uint8_t dat);
void LCD_WR_DATA(uint16_t dat);
void LCD_WR_REG(uint8_t dat);
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_Init(void);
void LCD_SetOrientation(uint8_t orient); /* runtime MADCTL rotation, 0..3 */
uint16_t LCD_GetWidth(void);             /* current orientation width */
uint16_t LCD_GetHeight(void);            /* current orientation height */

#ifdef __cplusplus
}
#endif

#endif /* LCD_INIT_H */
