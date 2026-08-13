/*
 * board.h - board/toolchain adaptation for the ST7735 0.96" TFT driver.
 *
 * The driver needs two platform things:
 *   - the STM32 register definitions (GPIOA/GPIOB, RCC)
 *   - a millisecond delay for init/reset timing
 *
 * Supported contexts:
 *   - Arduino / PlatformIO (ststm32, framework = arduino)
 *       -> uses <Arduino.h> and delay()
 *   - Bare-metal STM32 (ST's CMSIS device header)
 *       -> include "stm32f1xx.h", you must provide tft_delay_ms()
 *
 * Everything else in the driver is plain C, no library dependencies.
 */
#ifndef LCD_BOARD_H
#define LCD_BOARD_H

#if defined(ARDUINO)
  #include <Arduino.h>
  #define LCD_DELAY_MS(ms) delay(ms)
#elif defined(STM32F1) || defined(STM32F103xB) || defined(STM32F10X_MD) || \
      defined(STM32F10X_HD)
  #include "stm32f1xx.h"
  extern void tft_delay_ms(uint32_t ms); /* user provided */
  #define LCD_DELAY_MS(ms) tft_delay_ms(ms)
#else
  #error "board.h: define ARDUINO, or add your STM32 context here"
#endif

#endif /* LCD_BOARD_H */
