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
  LCD_Fill(0, 0, LCD_W, LCD_H, WHITE);
  LCD_ShowString(10, 10, "Hello!", RED, WHITE, 16, 0);
}

void loop(void) {
}
```

## Usage (bare-metal C)

Include `board.h` for a bare-metal STM32 context (provide `tft_delay_ms()`), or
edit `board.h` to add your own platform. Everything else is plain C.

## Orientation

`USE_HORIZONTAL` in `src/lcd_init.h`:

| Value | Orientation | Size | MADCTL | Window offsets |
| --- | --- | --- | --- | --- |
| 0, 1 | portrait | 80x160 | 0x08 / 0xC8 | x+26, y+1 |
| 2, 3 | landscape (default) | 160x80 | 0x78 / 0xA8 | x+1, y+26 |

## API

- `LCD_Init()` - power on + init the panel
- `LCD_Fill(x0, y0, x1, y1, color)` - solid fill
- `LCD_DrawPoint / LCD_DrawLine / LCD_DrawRectangle / Draw_Circle`
- `LCD_ShowChar / LCD_ShowString` - ASCII text (sizes 12/16/24/32)
- `LCD_ShowChinese / LCD_ShowChinese16x16` - Chinese characters
- `LCD_ShowIntNum / LCD_ShowFloatNum1` - numbers
- `LCD_ShowPicture(x, y, w, h, rgb565_hi_lo_bytes)` - images

Colors are RGB565; named colors live in `src/lcd.h`.

## Lessons learned (why this driver exists)

1. **Ask for the factory firmware early.** The code a board ships with is the
   ground truth: pinout, init, orientation, everything.
2. **The tooling layer can be the liar.** The board was never broken; the
   library's GPIO setup on that core was. Sometimes the register-level path is
   the *only* path that works.
3. ST-Link v2 clones sometimes fail on the first upload attempt. Just retry.

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
