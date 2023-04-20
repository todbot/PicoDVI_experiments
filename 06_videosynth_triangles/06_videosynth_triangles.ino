// PicoDVI 06_videosynth_triangles
// 18 Apr 2023 - @todbot / Tod Kurt
//
// Uses MIDI CC controls CC 36 to CC 41 (like the 8mu controller)
// 
// Must set:  Tools / USB Stack -> "Adafruit TinyUSB"
// 
// The original study for this is:
//  https://editor.p5js.org/todbot/sketches/DFLFuPYJC
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

int fade_amount = 8; // how fast previous frame fades away, higher = faster fade
const uint32_t millis_until_attract_mode = 10 * 1000;

const int midi_tx_pin = 0;  // MIDI Feather Wing
const int midi_rx_pin = 1;  

int16_t dw,dh;
uint32_t last_millis;
uint32_t last_millis_hue;
uint32_t last_millis_midi;
uint32_t draw_millis = 0;

const int shapes_cnt = 5;
ShapeThing shapes[shapes_cnt];


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  if (!display.begin()) { // Blink LED if insufficient RAM
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 100) & 1);
  }
  dw = display.width();
  dh = display.height();

  shapes_init(shapes, shapes_cnt, dw/2, dh/2);
  shapes[0].visible = true;
  for( int i=1; i<shapes_cnt; i++) { 
    shapes[i].cy = random(20,220);
    shapes[i].cx = random(20,300);
    shapes[i].rad = random(20,50);
    shapes[i].hue = random(0,255);
    shapes[i].sat = random(120,255);
    shapes[i].vang = 0.001 * random(1, 50);
  }
  
  Serial1.setRX(midi_rx_pin);
  Serial1.setTX(midi_tx_pin);

  MIDIusb.begin(MIDI_CHANNEL_OMNI);
  MIDIserial.begin(MIDI_CHANNEL_OMNI);

  MIDIusb.turnThruOff();    // turn off echo
  MIDIserial.turnThruOff(); // turn off echo

}

void loop() { 
  handleMIDIusb();
//  handleMIDIserial();  // FIXME: was this causing crashes? 
  do_video();
}


void handleControlChange(byte channel, byte cc_num, byte cc_val ) {
  Serial.printf("ccnum:%d val:%d\n", cc_num, cc_val);
  if( cc_num == 34 ) { // 8mu fader1
    shapes[0].hue = cc_val*2;
  }
  else if( cc_num == 35 ) { 
    shapes[0].cx = 10 + cc_val*2;
  }
  else if( cc_num == 40 ) { 
    int cnt = (shapes_cnt * cc_val)/127;
    for( int i=1; i<shapes_cnt; i++) { 
      shapes[i].visible = false; 
    }
    for(int i=1; i< cnt; i++) { 
      shapes[i].visible = true;
    }
  }
  else if( cc_num == 41 ) { // 8mu fader8
    for( int i=0; i<shapes_cnt; i++) {
      shapes[i].vang = 0.003 * cc_val;
    }
  }
}

//
void do_video() {
  // fade last frame
  // why 30 millis? seemed like a good idea,
  // many other magic variables sort of depend on this though
  if( millis() - last_millis > 30 ) {
    last_millis = millis();
    dim_buf16(&display, fade_amount, dw, dh); // fade last frame
    shapes_update(shapes, shapes_cnt);
  }
  
  // attract mode and auto hue shift
  if( millis() - last_millis_hue > 100 ) { 
    last_millis_hue = millis();
    Serial.println(draw_millis);
//    // attract mode if no MIDI
//    if( millis() - last_millis_midi > millis_until_attract_mode ) { 
//      shapes_attract();
//    }
  }

  uint32_t t = millis();
  
  shapes_draw(&display, shapes, shapes_cnt);
  
  draw_millis = millis() - t; // debug to test how long draws take
  
}

//
void handleMIDIusb() {
  while( MIDIusb.read() ) {  // use while() to read all pending MIDI, shouldn't hang
    switch(MIDIusb.getType()) {
      case midi::ControlChange:
        handleControlChange(0, MIDIusb.getData1(), MIDIusb.getData2());
        break;
      case midi::NoteOn:
        //handleNoteOn( 0, MIDIusb.getData1(),MIDIusb.getData2());
        break;
      case midi::NoteOff:
        //handleNoteOff( 0, MIDIusb.getData1(),MIDIusb.getData2());
        break;
      default:
        break;
    }
  }
}

// surely there's a way to have this generalized
void handleMIDIserial() {
  while( MIDIserial.read() ) {  // use while() to read all pending MIDI, shouldn't hang
    switch(MIDIserial.getType()) {
      case midi::ControlChange:
        handleControlChange(0, MIDIserial.getData1(), MIDIserial.getData2());
        break;
      case midi::NoteOn:
        //handleNoteOn( 0, MIDIserial.getData1(),MIDIserial.getData2());
        break;
      case midi::NoteOff:
        //handleNoteOff( 0, MIDIserial.getData1(),MIDIserial.getData2());
        break;
      default:
        break;
    }
  }
}
