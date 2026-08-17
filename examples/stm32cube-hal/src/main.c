/*
 * st7735-096tft-driver - STM32Cube HAL example.
 *
 * Same demo as examples/arduino-hal on the nologo STM32F103C8T6 0.96" TFT
 * board, built on ST's STM32Cube HAL. The driver's board.h detects
 * USE_HAL_DRIVER (defined automatically by PlatformIO's stm32cube framework)
 * and uses HAL_Delay() for init/reset timing; LCD_GPIO_Init() configures the
 * panel pins through HAL_GPIO_Init().
 *
 *   Build:  pio run
 *   Flash:  pio run -t upload   (ST-Link v2, 4-pin SWD header)
 *
 * Clock: HSI 8MHz -> PLL x16 -> SYSCLK 64MHz (no external crystal required).
 */
#include "lcd.h"
#include "stm32f1xx_hal.h"

void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}

/* HSI 8MHz -> PLL x16 -> SYSCLK 64MHz, APB1 = 32MHz, APB2 = 64MHz */
static void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType =
    RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

void SysTick_Handler(void) { HAL_IncTick(); }

void HardFault_Handler(void) {
  while (1) {
  }
}

int main(void) {
  HAL_Init();
  SystemClock_Config();

  /* PC13 user LED (bluepill) */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  LCD_Init();

  while (1) {
    uint16_t w = LCD_GetWidth();
    uint16_t h = LCD_GetHeight();

    /* solid color test */
    LCD_Fill(0, 0, w, h, RED);
    HAL_Delay(700);
    LCD_Fill(0, 0, w, h, GREEN);
    HAL_Delay(700);
    LCD_Fill(0, 0, w, h, BLUE);
    HAL_Delay(700);
    LCD_Fill(0, 0, w, h, WHITE);
    HAL_Delay(700);

    /* shapes on black */
    LCD_Fill(0, 0, w, h, BLACK);
    LCD_DrawRectangle(4, 4, w - 5, h - 5, WHITE);
    LCD_DrawLine(0, 0, w, h, MAGENTA);
    LCD_DrawLine(w, 0, 0, h, MAGENTA);
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

    HAL_Delay(2500);

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); /* blink user LED */
  }
}
