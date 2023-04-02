// PicoDVI 05_videosytnh_sine_midi
// 31 Mar 2023 - @todbot / Tod Kurt
//
// Must set:  Tools / USB Stack -> "Adafruit TinyUSB"

#include <PicoDVI.h> 

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

DVIGFX16 display(DVI_RES_320x240p60, adafruit_feather_dvi_cfg);

const int midi_tx_pin = 0;
const int midi_rx_pin = 1;

Adafruit_USBD_MIDI usb_midi;  // USB MIDI object
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDIusb); // USB MIDI
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIserial);   // Serial MIDI

int16_t dw,dh;
uint32_t last_millis = 0;
uint32_t last_millis_hue;
uint32_t last_millis_midi;

float noise_max = 10;
int fade_amount = 8; // how fast previous frame fades away, higher = faster fade

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  if (!display.begin()) { // Blink LED if insufficient RAM
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }
  dw = display.width();
  dh = display.height();

  shapes_init();
  
//  Serial1.setRX(midi_rx_pin);
//  Serial1.setTX(midi_tx_pin);

  MIDIusb.begin(MIDI_CHANNEL_OMNI);
//  MIDIserial.begin(MIDI_CHANNEL_OMNI);

  MIDIusb.turnThruOff();    // turn off echo
//  MIDIserial.turnThruOff(); // turn off echo

}

void loop() { 
  handleMIDIusb();
//  handleMIDIserial();
  do_video();
}

const uint8_t note_cnt = 3; // FIXME: match shape_cnt
uint8_t notes_on[note_cnt];
uint8_t curr_note = 0;
//
void handleNoteOn(byte channel, byte note, byte vel) {
  Serial.printf("handleNoteOn! note:%d vel:%d\n", note, vel);
  digitalWrite(LED_BUILTIN,HIGH);
  last_millis_midi = millis();
  curr_note = (curr_note + 1) % note_cnt;
  notes_on[curr_note] = note;
  shapes_shape_on(curr_note, note, vel);
}
//
void handleNoteOff(byte channel, byte note, byte vel) {
  Serial.printf("handleNoteOff! note:%d vel:%d\n", note, vel);
  digitalWrite(LED_BUILTIN,LOW);
  for( int i=0; i<note_cnt; i++) { 
    if( notes_on[i] == note ) { 
      shapes_shape_off(i, note, vel);      
    }
  }
}

//
void do_video() {
  // fade last frame
  if( millis() - last_millis > 30 ) { 
    last_millis = millis();
    dim_buf16(&display, fade_amount); // fade last frame
    shapes_update();
  }
  
  // attract mode and auto hue shift
  if( millis() - last_millis_hue > 100 ) { 
    last_millis_hue = millis();
    if( millis() - last_millis_midi > 5000 ) { // attract mode if no MIDI
      shapes_attract();
    }
  }

  shapes_draw(&display);
  
}
