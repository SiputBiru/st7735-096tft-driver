/*
 * st7735-096tft-driver demo - nologo STM32F103C8T6 0.96" TFT
 * Shows fills, shapes, text, and numbers on the 160x80 landscape panel.
 *
 *   Build:  pio run
 *   Flash:  pio run -t upload   (ST-Link v2, 4-pin SWD header)
 */
#include "lcd.h"
#include <Arduino.h>

void setup() {
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH);

  LCD_Init();
}

void loop() {
  uint16_t w = LCD_GetWidth();
  uint16_t h = LCD_GetHeight();

  /* solid color test */
  LCD_Fill(0, 0, w, h, RED);
  delay(700);
  LCD_Fill(0, 0, w, h, GREEN);
  delay(700);
  LCD_Fill(0, 0, w, h, BLUE);
  delay(700);
  LCD_Fill(0, 0, w, h, WHITE);
  delay(700);

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

  delay(2500);

  digitalWrite(PC13, !digitalRead(PC13)); /* blink user LED */
}
