// PicoDVI 06_videosynth_triangles -- experiments with longer persistence
// 18 Apr 2023 - @todbot / Tod Kurt
//
// Uses MIDI CC controls CC 36 to CC 41 (like the 8mu controller)
//
// Must set:  Tools / USB Stack -> "Adafruit TinyUSB"
//
// The study for this in p5js is:
//   https://openprocessing.org/sketch/1906251
//

#include <PicoDVI.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

#include "picodvi_utils.h"
#include "shapething.h"

DVIGFX16 display(DVI_RES_320x240p60, adafruit_feather_dvi_cfg);
//DVIGFX16 display(DVI_RES_640x480p60, adafruit_feather_dvi_cfg); // not enough RAM?

Adafruit_USBD_MIDI usb_midi;  // USB MIDI object
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDIusb); // USB MIDI
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIserial);   // Serial MIDI

int fade_amount = 8; // how fast previous frame fades away, higher = faster fade, 8 is minimum

const int midi_tx_pin = 0;  // MIDI Feather Wing
const int midi_rx_pin = 1;

int16_t dw,dh;
uint32_t last_update_millis;
uint32_t last_fade_millis;
uint32_t draw_millis = 0;

uint16_t fade_millis = 40;

const int shapes_cnt = 6;
ShapeThing shapes[shapes_cnt];


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  if (!display.begin()) { // Blink LED if insufficient RAM
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 100) & 1);
  }
  dw = display.width();
  dh = display.height();

  shapes_init(shapes, shapes_cnt);
  for( int i=0; i<shapes_cnt; i++) {
    shapes[i].cy = random(20,220);
    shapes[i].cx = random(20,300);
    shapes[i].cr = random(100,150);
    shapes[i].hue = random(0,255);
    shapes[i].sat = random(0,120);
    shapes[i].vang = 0.001 * random(1, 50);
    shapes[i].vrad = 0.001 * random(1, 50);
  }
  shapes[0].visible = true;
  shapes[0].cx = dw/2;
  shapes[0].cy = dh/2;
  shapes[0].cr = dw/3;

  Serial1.setRX(midi_rx_pin);
  Serial1.setTX(midi_tx_pin);

  MIDIusb.setHandleControlChange(handleControlChange);
  MIDIserial.setHandleControlChange(handleControlChange);

  MIDIusb.begin(MIDI_CHANNEL_OMNI);
  MIDIserial.begin(MIDI_CHANNEL_OMNI);

  MIDIusb.turnThruOff();    // turn off echo
  MIDIserial.turnThruOff(); // turn off echo

}

void loop() {
  MIDIusb.read();

  do_video();
}

//
void do_video() {
  // fade last frame
  if( millis() - last_fade_millis > fade_millis ) {
    last_fade_millis = millis();
    dim_buf16(&display, fade_amount, dw, dh); // fade last frame
  }
  // why 30 millis? seemed like a good idea,
  // many other magic variables sort of depend on this though
  if( millis() - last_update_millis > 30 ) {
    last_update_millis = millis();
    shapes_update(shapes, shapes_cnt);
  }

  uint32_t t = millis();

  shapes_draw(&display, shapes, shapes_cnt);

  draw_millis = millis() - t; // debug to test how long draws take

}

// called by MIDI.read()
void handleControlChange(byte channel, byte cc_num, byte cc_val ) {
  (void)channel; // unused
  Serial.printf("ccnum:%d :%d\n", cc_num, cc_val);
  if( cc_num == 34 ) { // 8mu fader1
    shapes[0].hue = cc_val*2;
  }
  else if( cc_num == 35 ) {
    shapes[0].cr = 10 + cc_val/2;
  }
  /* else if( cc_num == 36 ) { */  // too distracting
  /*   shapes[0].cx = 30 + cc_val*3; */
  /* } */
  else if( cc_num == 37 ) {
    fade_millis = cc_val*2 + 1; // map( cc_val, 0,127, 1, 100);  // fade amount
  }
  else if( cc_num == 39 ) {
    int cnt = (shapes_cnt * cc_val)/127;
    for( int i=1; i<shapes_cnt; i++) {
      shapes[i].visible = false;
    }
    for(int i=1; i< cnt; i++) {
      shapes[i].visible = true;
    }
  }
  else if( cc_num == 40 ) {
    for( int i=0; i<shapes_cnt; i++) {
      shapes[i].vrad = 1 + 10 * cc_val;  // radius changing
    }
  }
  else if( cc_num == 41 ) { // 8mu fader8
    for( int i=0; i<shapes_cnt; i++) {
      shapes[i].vang = 0.003 * cc_val;  // rotational speed
    }
  }
}
