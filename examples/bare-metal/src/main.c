/*
 * st7735-096tft-driver - bare-metal STM32 example (CMSIS, no framework).
 *
 * Same demo as examples/arduino-hal on the nologo STM32F103C8T6 0.96" TFT
 * board, but with no framework at all: just the CMSIS device header and
 * direct peripheral registers. The driver's board.h detects the bare-metal
 * STM32F1 context (the board defines STM32F1/STM32F103xB) and calls the
 * tft_delay_ms() provided here.
 *
 *   Build:  pio run
 *   Flash:  pio run -t upload   (ST-Link v2, 4-pin SWD header)
 *
 * Clock: HSI 8MHz -> PLL x16 -> SYSCLK 64MHz (no external crystal required).
 * Delay: SysTick 1ms tick -> tft_delay_ms().
 */
#include "lcd.h"
#include "stm32f1xx.h"

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

int main(void) {
  SystemClock_Config();
  SysTick_Config(SystemCoreClock / 1000); /* 1ms tick */

  /* PC13 user LED (bluepill) as 50MHz push-pull output */
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  GPIOC->CRH |= GPIO_CRH_MODE13;

  LCD_Init();

  while (1) {
    /* solid color test */
    LCD_Fill(0, 0, LCD_W, LCD_H, RED);
    tft_delay_ms(700);
    LCD_Fill(0, 0, LCD_W, LCD_H, GREEN);
    tft_delay_ms(700);
    LCD_Fill(0, 0, LCD_W, LCD_H, BLUE);
    tft_delay_ms(700);
    LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
    tft_delay_ms(700);

    /* shapes on black */
    LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
    LCD_DrawRectangle(4, 4, LCD_W - 5, LCD_H - 5, WHITE);
    LCD_DrawLine(0, 0, LCD_W - 1, LCD_H - 1, MAGENTA);
    LCD_DrawLine(LCD_W - 1, 0, 0, LCD_H - 1, MAGENTA);
    Draw_Circle(80, 40, 30, CYAN);

    /* text (8x16 font) */
    LCD_ShowString(24, 8, "NOLOGE!!!", RED, BLACK, 16, 0);
    LCD_ShowString(24, 28, "0.96 IPS TFT", YELLOW, BLACK, 16, 0);
    LCD_ShowString(24, 48, "st7735 driver", GREEN, BLACK, 16, 0);
    LCD_ShowString(24, 68, "REGISTER SPI", CYAN, BLACK, 16, 0);

    /* numbers */
    LCD_ShowString(24, 92, "int :", LGRAY, BLACK, 16, 0);
    LCD_ShowIntNum(84, 92, 1234, 4, WHITE, BLACK, 16);
    LCD_ShowString(24, 112, "flt :", LGRAY, BLACK, 16, 0);
    LCD_ShowFloatNum1(84, 112, 3.14f, 4, WHITE, BLACK, 16);

    tft_delay_ms(2500);

    GPIOC->ODR ^= (1u << 13); /* blink user LED */
  }
}
