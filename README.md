# st7735-096tft-driver

ST7735S **0.96 inch (80x160)** TFT display driver for **STM32F103**, bit-banged
through direct GPIO registers. Zero dependencies. Fills, lines, circles,
rectangles, ASCII text, Chinese characters, and pictures.

Ported **verbatim** from the nologo factory firmware (the code the
STM32F103C8T6 0.96" TFT board ships with).

## Why direct registers?

The usual route (Adafruit ST7735 + Arduino's software SPI) silently fails on the
STM32duino core: `pinMode(OUTPUT)` defaults to a 2 MHz drive, and the resulting
clock edges are too soft for the ST7735 to latch. nologo's factory driver configures
the pins as **50 MHz push-pull** and toggles `GPIOx->BSRR/BRR` directly, one
instruction per bit. That is what this library does, and it works.

## Wiring (nologo STM32F103C8T6 0.96" TFT board, factory-verified)

| Signal | Pin |
| --- | --- |
| SCK | PB10 |
| MOSI (SDA) | PB11 |
| RES | PA7 |
| DC | PB0 |
| CS | PB1 |
| BLK (backlight, active low) | PA6 |

To change pins, edit the `LCD_*` macros at the top of `src/lcd_init.h`.

more info in [nologo docs](https://www.nologo.tech/en/product/stm32/STM32F103C8T6-C6T60.96TFT/stm32f103rct61.html).

## Usage (PlatformIO / Arduino)

Add to `platformio.ini`:

```ini
lib_deps =
    file:///path/to/st7735-096tft-driver
```

Or just copy `src/` into your project. Then:

```c
#include "lcd.h"

void setup(void) {
  LCD_Init();                 // init display + backlight
  LCD_Fill(0, 0, LCD_GetWidth(), LCD_GetHeight(), WHITE);
  LCD_ShowString(10, 10, "Hello!", RED, WHITE, 16, 0);
}

void loop(void) {
}
```

## Bare-metal usage

Include `board.h` for a bare-metal STM32 context (provide `tft_delay_ms()`), or
edit `board.h` to add your own platform. The driver has no framework
dependencies: `board.h` only needs the STM32 register definitions
(`stm32f1xx.h`) and a millisecond delay.

```c
#include "lcd.h"     // pulls in board.h -> stm32f1xx.h

void tft_delay_ms(uint32_t ms);   // you provide this (e.g. SysTick-based)

int main(void) {
  /* set up your clock + SysTick, then: */
  LCD_Init();
  LCD_Fill(0, 0, LCD_GetWidth(), LCD_GetHeight(), WHITE);
  while (1) { }
}
```

## STM32Cube HAL usage

The driver also works under ST's STM32Cube HAL. `board.h` detects
`USE_HAL_DRIVER` (defined automatically by PlatformIO's `stm32cube`
framework) and uses `HAL_Delay()`; `LCD_GPIO_Init()` configures the panel
pins through `HAL_GPIO_Init()` instead of raw registers. Call `HAL_Init()`
(so SysTick runs) before `LCD_Init()`:

```c
#include "stm32f1xx_hal.h"
#include "lcd.h"

int main(void) {
  HAL_Init();
  /* SystemClock_Config() ... */
  LCD_Init();
  LCD_Fill(0, 0, LCD_GetWidth(), LCD_GetHeight(), WHITE);
  while (1) { }
}
```

## Examples

Four examples for the nologo STM32F103C8T6 0.96" TFT board, all flashing
with an ST-Link v2 (`pio run -t upload`):

| Directory | Framework | Entry point | Delay |
| --- | --- | --- | --- |
| `examples/arduino-hal` | Arduino (`framework = arduino`) | `setup()` / `loop()` | `delay()` |
| `examples/bare-metal` | CMSIS, no framework (`framework = cmsis`) | `main()` | SysTick → `tft_delay_ms()` |
| `examples/stm32cube-hal` | STM32Cube HAL (`framework = stm32cube`) | `main()` | `HAL_Delay()` |
| `examples/bare-metal-text-rotate` | CMSIS, no framework | `main()` | SysTick → `tft_delay_ms()` |

The three C examples run at 64 MHz from the internal HSI oscillator (PLL x16),
so no external crystal is required.

`bare-metal-text-rotate` shows "st7735 driver" centered while cycling the
panel through all 4 native rotations at runtime (`LCD_SetOrientation`).

## Orientation

`USE_HORIZONTAL` in `src/lcd_init.h` selects the compile-time default; the
runtime API uses the `lcd_orientation_t` enum:

| Enum | Value | Orientation | Size | MADCTL | Window offsets |
| --- | --- | --- | --- | --- | --- |
| `LCD_PORTRAIT` | 0 | portrait | 80x160 | 0x08 | x+26, y+1 |
| `LCD_PORTRAIT_FLIP` | 1 | portrait (flipped) | 80x160 | 0xC8 | x+26, y+1 |
| `LCD_LANDSCAPE` | 2 | landscape (default) | 160x80 | 0x78 | x+1, y+26 |
| `LCD_LANDSCAPE_FLIP` | 3 | landscape (flipped) | 160x80 | 0xA8 | x+1, y+26 |

## API

- `LCD_Init()` - power on + init the panel
- `LCD_SetOrientation(lcd_orientation_t)` - rotate the panel at runtime (MADCTL)
- `LCD_GetOrientation()` - current orientation (`lcd_orientation_t`)
- `LCD_GetWidth() / LCD_GetHeight()` - current orientation size
- `LCD_Fill(x0, y0, x1, y1, color)` - solid fill
- `LCD_DrawPoint / LCD_DrawLine / LCD_DrawRectangle / Draw_Circle`
- `LCD_ShowChar / LCD_ShowString` - ASCII text (sizes 12/16/24/32)
- `LCD_ShowChinese / LCD_ShowChinese16x16` - Chinese characters
- `LCD_ShowIntNum / LCD_ShowFloatNum1` - numbers
- `LCD_ShowPicture(x, y, w, h, rgb565_hi_lo_bytes)` - images

Colors are RGB565; named colors live in `src/lcd.h`. The `LCD_W`/`LCD_H`
macros are compile-time constants for the default orientation; prefer
`LCD_GetWidth()/LCD_GetHeight()` in code that runs after
`LCD_SetOrientation()`.

## Font sizes & limitations

The glyphs are pre-rendered bitmaps baked into `src/lcdfont.h`; the driver
cannot scale them. Only the sizes below exist, and the width is always
`sizey / 2`:

| `sizey` | `sizex` | ASCII table | Chinese table |
| --- | --- | --- | --- |
| 12 | 6 | `ascii_1206[][12]` | - |
| 16 | 8 | `ascii_1608[][16]` | `tfont16[]` (16x16) |
| 24 | 12 | `ascii_2412[][48]` | - |
| 32 | 16 | `ascii_3216[][64]` | - |

### ASCII (`LCD_ShowChar` / `LCD_ShowString`)

Passing any other `sizey` hits `else return;` in `LCD_ShowChar` and draws
nothing. The screen keeps whatever background you filled (typically a black
screen). There is no error message and no fallback.

### Chinese (`LCD_ShowChinese` / `LCD_ShowChinese16x16`)

The Chinese font is fixed at 16x16 (`tfont16[]`, 32 bytes per glyph) and has
no size guard at all. `sizey` is used to compute the byte count, so:

- `sizey < 16` reads too few bytes and misaligns rows, producing garbage glyphs
- `sizey > 16` reads past the 32-byte glyph (`Msk[32]`), an out-of-bounds
  read; for the last character in the table it reads past the end of
  `tfont16[]` entirely, which can hard-fault the MCU

### Adding new sizes

The driver only knows the four tables above. To use other sizes, either scale
the existing bitmaps (draw each font pixel as an NxN block) or generate new
font tables and extend the `if/else` chain in `LCD_ShowChar`.

## Credits / Provenance

- The **init sequence** (register values) and the **overall driver structure**
  are ported from **nologo's factory firmware** (the `stm32c8t6` factory test
  that ships with this board). nologo publishes this code without an explicit
  license. Board docs: https://www.nologo.tech/en/
- The **drawing functions** (Bresenham line, midpoint circle, font renderer)
  are classic, widely-published algorithms.
- The **fonts** (`ascii_1206/1608/2412/3216`, 16x16 Chinese) are the ubiquitous
  bitmaps shared across embedded tutorials; their exact origin is unclear.

This library is a **personal / educational** project. If you plan to distribute
it commercially, please reach out to nologo about their example firmware first.

## License

MIT for this repository's own contributions (porting, `board.h`, build
manifests, docs). See [LICENSE](LICENSE). The nologo-derived portions are used
under the provenance note above.
