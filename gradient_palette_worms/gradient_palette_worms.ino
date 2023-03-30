// PicoDVI gradient palette worms
// 29 Mar 2023 - @todbot / Tod Kurt
// Edited verion of https://github.com/adafruit/PicoDVI/blob/master/examples/8bit_double_buffer/8bit_double_buffer.ino

// Double-buffered 8-bit Adafruit_GFX-compatible framebuffer for PicoDVI.
// Animates without redraw flicker. Requires Adafruit_GFX >= 1.11.4

#include <PicoDVI.h>

// Here's how a 320x240 8-bit (color-paletted) framebuffer is declared.
// Second argument ('true' here) enables double-buffering for flicker-free
// animation. Third argument is a hardware configuration -- examples are
// written for Adafruit Feather RP2040 DVI, but that's easily switched out
// for boards like the Pimoroni Pico DV (use 'pimoroni_demo_hdmi_cfg') or
// Pico DVI Sock ('pico_sock_cfg').
DVIGFX8 display(DVI_RES_320x240p60, true, adafruit_feather_dvi_cfg);

int16_t dw,dh;

const int num_balls = 20; // Number of bouncy balls to draw, 1-254 (not 255)
const int ball_size = 20; // median size of balls
//#define num_balls 20 
//#define ball_size 20 

struct { 
  float x,y;   // position   we use floats in this house!
  float vx,vy; // velocity
  uint8_t colorid; // palette id
  uint8_t r; // ball size
} ball[num_balls];


void setup() {
  if (!display.begin()) { // Blink LED if insufficient RAM
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }
  dw = display.width();
  dh = display.height();

  make_rainbow_palette(&display);
  //make_websafe_palette(&display);
  //make_rgb_palette(&display);
  //make_random_4color_palette(&display);
  //make_random_ncolor_palette(&display, random(255), 8);

  // Randomize initial ball positions, velocities and colors
  for (int i=0; i<num_balls; i++) {
    //ball[i].colorid = random(4)*64 -1 ; 
    ball[i].colorid = random(255); // pick a random color from 0-255 palette
    ball[i].r = random(ball_size-10, ball_size+10);
    ball[i].x = ball[i].r + random(display.width() - ball[i].r);
    ball[i].y = ball[i].r + random(display.height() - ball[i].r);
    ball[i].vx = random(20)/10.0;
    ball[i].vy = random(20)/10.0;
  }
  display.swap(false, true); // Duplicate same palette into front & back buffers
}

uint32_t last_millis = 0;

void loop() {
  //display.fillScreen(0);
  
//  if( millis() - last_millis > 5000 ) { 
//    last_millis = millis();
//    make_random_ncolor_palette(8);  
//  }

  dim_buf8(&display, 1);  

  for (int i=0; i<num_balls; i++) {
    display.fillCircle(ball[i].x, ball[i].y, ball[i].r, ball[i].colorid );
    
    // update position
    ball[i].x += ball[i].vx;
    ball[i].y += ball[i].vy;
    
    // bounce! and update velocity
    if ( (ball[i].x <= ball[i].r) || (ball[i].x >= dw-ball[i].r) ) ball[i].vx *= -1;
    if ( (ball[i].y <= ball[i].r) || (ball[i].y >= dh-ball[i].r) ) ball[i].vy *= -1;
  }
  
  // Swap front/back buffers, do not duplicate current screen state to next frame,
  // we'll draw it new from scratch each time.
  display.swap();
}
