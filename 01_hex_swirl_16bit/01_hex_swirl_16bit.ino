// PicoDVI hex swirl
// 30 Mar 2023 - @todbot / Tod Kurt
// Edited verion of https://github.com/adafruit/PicoDVI/blob/master/examples/16bit_hello/16bit_hello.ino
//
// Basic full-color PicoDVI test. Provides a 16-bit color video framebuffer to
// which Adafruit_GFX calls can be made. It's based on the EYESPI_Test.ino sketch.

#include <PicoDVI.h>                  // Core display & graphics library

// Here's how a 320x240 16-bit color framebuffer is declared. Double-buffering
// is not an option in 16-bit color mode, just not enough RAM; all drawing
// operations are shown as they occur. Second argument is a hardware
// configuration -- examples are written for Adafruit Feather RP2040 DVI, but
// that's easily switched out for boards like the Pimoroni Pico DV (use
// 'pimoroni_demo_hdmi_cfg') or Pico DVI Sock ('pico_sock_cfg').
DVIGFX16 display(DVI_RES_320x240p60, adafruit_feather_dvi_cfg);

int16_t dw,dh;
float bm_x = 0;
float bm_y = 0;
uint32_t last_millis = 0;

void setup() { // Runs once on startup
  if (!display.begin()) { // Blink LED if insufficient RAM
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }
  dw = display.width();
  dh = display.height();
}

void loop() {

  show_bitmap_tod();
  
}

// BITMAP EXAMPLE ----------------------------------------------------------

// This section demonstrates:
// - Embedding a small bitmap in the code (flash memory).
// - Drawing that bitmap in various colors, and transparently (only '1' bits
//   are drawn; '0' bits are skipped, leaving screen contents in place).
// - Use of the color565() function to decimate 24-bit RGB to 16 bits.

#define HEX_WIDTH  16 // Bitmap width in pixels
#define HEX_HEIGHT 16 // Bitmap height in pixels
// Bitmap data. PROGMEM ensures it's in flash memory (not RAM). And while
// it would be valid to leave the brackets empty here (i.e. hex_bitmap[]),
// having dimensions with a little math makes the compiler verify the
// correct number of bytes are present in the list.
PROGMEM const uint8_t hex_bitmap[(HEX_WIDTH + 7) / 8 * HEX_HEIGHT] = {
  0b00000001, 0b10000000,
  0b00000111, 0b11100000,
  0b00011111, 0b11111000,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b00011111, 0b11111000,
  0b00000111, 0b11100000,
  0b00000001, 0b10000000,
};
//#define Y_SPACING (HEX_HEIGHT - 2) // Used by code below for positioning
#define Y_SPACING (HEX_HEIGHT * 1) // Used by code below for positioning

void show_bitmap_tod() {
  // display.fillScreen(0);
  
  dim_buf16(&display, 8); // dim last frame by 8,8,8
  
  // if( millis() - last_millis < 1000 ) { return; } // to test dimming
  // last_millis = millis();

  // Not screen center, but UL coordinates of center hexagon bitmap
  const int16_t center_x = bm_x + (display.width() - HEX_WIDTH) / 2;
  const int16_t center_y = bm_y + (display.height() - HEX_HEIGHT) / 2;
  const uint8_t steps = min((display.height() - HEX_HEIGHT) / Y_SPACING,
                             display.width() / HEX_WIDTH - 1) / 2;
  bm_x = sin(millis()/1000.0) * 100;
  bm_y = cos(millis()/1000.0) * 100;

  display.drawBitmap(center_x, center_y, hex_bitmap, HEX_WIDTH, HEX_HEIGHT, 0xffff); // Draw center hexagon in white
   
  // Tile the hexagon bitmap repeatedly in a range of hues. Don't mind the
  // bit of repetition in the math, the optimizer easily picks this up.
  // Also, if math looks odd, keep in mind "PEMDAS" operator precedence;
  // multiplication and division occur before addition and subtraction.
  for (uint8_t a=0; a<=steps; a++) {
    for (uint8_t b=1; b<=steps; b++) {
      display.drawBitmap( // Right section centered red: a = green, b = blue
        center_x + (a + b) * HEX_WIDTH / 2,
        center_y + (a - b) * Y_SPACING,
        hex_bitmap, HEX_WIDTH, HEX_HEIGHT,
        display.color565(255, 255 - 255 * a / steps, 255 - 255 * b / steps));
      display.drawBitmap( // UL section centered green: a = blue, b = red
        center_x - b * HEX_WIDTH + a * HEX_WIDTH / 2,
        center_y - a * Y_SPACING,
        hex_bitmap, HEX_WIDTH, HEX_HEIGHT,
        display.color565(255 - 255 * b / steps, 255, 255 - 255 * a / steps));
      display.drawBitmap( // LL section centered blue: a = red, b = green
        center_x - a * HEX_WIDTH + b * HEX_WIDTH / 2,
        center_y + b * Y_SPACING,
        hex_bitmap, HEX_WIDTH, HEX_HEIGHT,
        display.color565(255 - 255 * a / steps, 255 - 255 * b / steps, 255));
    }
  }

}
