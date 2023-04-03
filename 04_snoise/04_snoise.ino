// PicoDVI 04_snoise
// 31 Mar 2023 - @todbot / Tod Kurt
// based on https://github.com/adafruit/PicoDVI/blob/master/examples/16bit_hello/16bit_hello.ino
//
// Creates a colorful spinning pattern based on SimplexNoise noise loops
//
// potA - controls spin rate
// potB - controls noise density
// potC - controls frame fade amount
//
// FIXME: why is the noise output radially symmetric? 

#include <PicoDVI.h> 

#include "SimplexNoise.h"

DVIGFX16 display(DVI_RES_320x240p60, adafruit_feather_dvi_cfg);

SimplexNoise sn;

const int potA_pin = A0;
const int potB_pin = A1;
const int potC_pin = A2;
const int but_pin = 24;

int16_t dw,dh;
uint32_t last_millis = 0;

uint8_t r,g,b; 
uint8_t hue=0, sat=255;
int butval = 0; // 
int butval_inc = 1;
uint32_t last_millis_hue;

float potA = 0, potB = 0, potC = 0;
float ease = 0.90;
uint32_t last_millis_inputs;

float phase = 0;
float phase_inc = 0.01;
float noise_max = 1;
int fade_amount = 5;

void setup() {
  if (!display.begin()) { // Blink LED if insufficient RAM
    pinMode(LED_BUILTIN, OUTPUT);
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }
  dw = display.width();
  dh = display.height();

  pinMode(but_pin, INPUT_PULLUP);
}

void loop() { 
  read_inputs();
  do_video();
}

void read_inputs() {
  // read every 10 millis
  if( millis() - last_millis_inputs > 10 ) { 
    last_millis_inputs = millis();
    int a = analogRead(potA_pin);
    int b = analogRead(potB_pin);
    int c = analogRead(potC_pin);
    int but = digitalRead(but_pin);
    
    // simple filtering so sudden movements aren't jarring
    potA = ease * potA + (1-ease) * a;
    potB = ease * potB + (1-ease) * b;
    potC = ease * potC + (1-ease) * c;
    
    phase_inc = 0.001 + 0.1*(potA/1024.0);
    noise_max = 100 * (potB/1024.0);
    fade_amount = 20 * (potC/1024.0);
    
    if( but == LOW ) { // pressed
      butval += butval_inc;
      if( butval >= 100 || butval <= 0 ) { butval_inc = -butval_inc; }
      Serial.printf("butval:%d\n", butval);
    }
  }
}

void do_video() {

  // fade last frame
  if( millis() - last_millis > 30 ) { 
    last_millis = millis();
    dim_buf16(&display, fade_amount); // fade last frame
  }
  
  // auto hue shift
  if( millis() - last_millis_hue > 100 ) { 
    last_millis_hue = millis();
    hue++;
    //Serial.printf("noise:%1.2f\n", sn.noise(millis()/1000.0, 0) );
  }

  float cx = (dw/2); 
  float cy = (dh/2); 
  float circler = 5;
  
  for(float a = 0; a < TWO_PI; a += 0.1 ) { // 0.087 = ~radians(5)

    float xoff = 10 + (1 + cos(a + phase)) * noise_max/2;
    float yoff = 10 + (1 + sin(a + phase)) * noise_max/2;
    float br = (1 + sn.noise(xoff, yoff)) * 50 + 10; // 10 = min radius, 50 = range of radius
    float bx = br * cos(a);
    float by = br * sin(a);

    hsbtorgb(&r, &g, &b,  hue, sat, 255);
  
    display.fillCircle( cx + bx, cy + by, circler, display.color565(r,g,b) );  
  }
  phase += phase_inc;

}
