/*
 * st7735-096tft-driver - text rotation demo (bare-metal STM32, CMSIS).
 *
 * Cycles the panel through its 4 native rotations (MADCTL 0x08/C8/78/A8).
 * Each rotation is one "refresh": clear with a color, draw "st7735 driver"
 * centered for the current orientation, pause, then rotate 90 degrees and
 * repeat. Uses LCD_SetOrientation(), which rotates at runtime.
 *
 *   Build:  pio run
 *   Flash:  pio run -t upload   (ST-Link v2, 4-pin SWD header)
 *
 * Clock: HSI 8MHz -> PLL x16 -> SYSCLK 64MHz (no external crystal required).
 * Delay: SysTick 1ms tick -> tft_delay_ms().
 */
#include "stm32f1xx.h"

#include "lcd.h"

static volatile uint32_t g_tick;

void SysTick_Handler(void) { g_tick++; }

/* the driver's bare-metal board.h context requires this symbol */
void tft_delay_ms(uint32_t ms) {
  uint32_t start = g_tick;
  while ((g_tick - start) < ms)
    ;
}

/* HSI 8MHz -> PLL x16 -> SYSCLK 64MHz, APB1 = 32MHz, APB2 = 64MHz */
static void SystemClock_Config(void) {
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2; /* 2 wait states @ 64MHz */

  RCC->CR |= RCC_CR_HSION; /* HSI on (default after reset) */
  while (!(RCC->CR & RCC_CR_HSIRDY))
    ;

  RCC->CFGR = RCC_CFGR_PLLMULL16 | RCC_CFGR_PPRE1_DIV2; /* PLL src = HSI/2, APB1 = /2 */

  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  RCC->CFGR |= RCC_CFGR_SW_PLL; /* SYSCLK = PLL */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;

  SystemCoreClockUpdate(); /* 64000000, for SysTick_Config below */
}

#define TEXT "st7735 driver"
#define TEXT_LEN (sizeof(TEXT) - 1u) /* 13 chars */
#define FONT_H 12u                   /* sizey: 6x12 font */
#define FONT_W 6u                    /* sizey / 2 */

/* draw the text centered in the current orientation */
static void ShowCentered(uint16_t fc, uint16_t bc) {
  uint16_t x = (LCD_GetWidth() - TEXT_LEN * FONT_W) / 2;
  uint16_t y = (LCD_GetHeight() - FONT_H) / 2;
  LCD_ShowString(x, y, TEXT, fc, bc, FONT_H, 0);
}

int main(void) {
  static const uint16_t color[4] = { RED, GREEN, BLUE, GRAY };
  lcd_orientation_t o;

  SystemClock_Config();
  SysTick_Config(SystemCoreClock / 1000); /* 1ms tick */

  LCD_Init();

  while (1) {
    for (o = LCD_PORTRAIT; o <= LCD_LANDSCAPE_FLIP; o++) {
      uint16_t w, h;

      LCD_SetOrientation(o); /* rotate 90 degrees */
      w = LCD_GetWidth();
      h = LCD_GetHeight();

      /* refresh */
      LCD_Fill(0, 0, w, h, color[o]);

      /* text, centered for the current orientation */
      ShowCentered(WHITE, BLACK);

      /* rotation label, top-left corner */
      LCD_ShowIntNum(2, 2, o, 1, BLACK, WHITE, 16);

      tft_delay_ms(1500);
    }
  }
}
