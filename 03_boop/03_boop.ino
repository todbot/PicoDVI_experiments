// PicoDVI 03_boop
// 30 Mar 2023 - @todbot / Tod Kurt
// based on https://github.com/adafruit/PicoDVI/blob/master/examples/16bit_hello/16bit_hello.ino
//
// Creates a colorful iris effect, based on 02_circle_wub

#include <PicoDVI.h>                  // Core display & graphics library

DVIGFX16 display(DVI_RES_320x240p60, adafruit_feather_dvi_cfg);

const int potA_pin = A0;
const int potB_pin = A1;
const int potC_pin = A2;
const int but_pin = 24;

int16_t dw,dh;
uint32_t last_millis = 0;

int num_balls = 50;
int curr_ball = 0;

uint8_t r,g,b; 
uint8_t hue, sat;
float ball_mod = 0;
int butval = 0; // 
int butval_inc = 1;
uint32_t last_millis_hue;

float potA = 0, potB = 0, potC = 0;
float ease = 0.90;
uint32_t last_millis_inputs;

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
    //ball_mod = 30 *  (potA / 1024.0);
    sat = 255 * (potB / 1024.0);
    //num_balls = 2 + 15 * (potC/1024.0);
    
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
    dim_buf16(&display, 4); // dim last frame by 4,4,4
  }
  
  // auto hue shift
  if( millis() - last_millis_hue > 100 ) { 
    last_millis_hue = millis();
    hue++;
  }

  // dimensions of ball
  int circler = 15 + (sin(millis()/5700.0)) * 2 + 40*(butval/100.0);  

  // distance between balls
  ball_mod = 25.0 * (0.5+sin(millis()/1000.0)); // 0.5 = allow ball_mod to go a litle negative
  int spacingr = 25 + ball_mod*5;
  
  // center point of screen 
  float cx = (dw/2); 
  float cy = (dh/2); 

  float ball_speed = 1700;
  // position of this ball from center point
  float bx = sin(millis()/ball_speed + curr_ball*(6.28/num_balls) ) * spacingr;
  float by = cos(millis()/ball_speed + curr_ball*(6.28/num_balls) ) * spacingr;

  // get RGB for this ball
  hsbtorgb(&r, &g, &b,  hue + (curr_ball*256/num_balls), sat, 255);
  
  display.fillCircle( cx + bx, cy + by, circler, display.color565(r,g,b) );
  
  curr_ball = (curr_ball+1) % num_balls;  // go to next ball
 
}
