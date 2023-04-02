// PicoDVI 05_videosytnh_sine_midi
// 31 Mar 2023 - @todbot / Tod Kurt
//
// Must set:  Tools / USB Stack -> "Adafruit TinyUSB"
// 

#include <PicoDVI.h> 

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

DVIGFX16 display(DVI_RES_320x240p60, adafruit_feather_dvi_cfg);

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

// for note-stealing algorithm in noteOn()
const uint8_t notes_cnt = 6; //shapes_cnt; // FIXME: match shapes_cnt
uint8_t notes_on[notes_cnt];
uint32_t notes_millis[notes_cnt];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  if (!display.begin()) { // Blink LED if insufficient RAM
    for (;;) digitalWrite(LED_BUILTIN, (millis() / 500) & 1);
  }
  dw = display.width();
  dh = display.height();

  shapes_init();
  
  Serial1.setRX(midi_rx_pin);
  Serial1.setTX(midi_tx_pin);

  MIDIusb.begin(MIDI_CHANNEL_OMNI);
  MIDIserial.begin(MIDI_CHANNEL_OMNI);

  MIDIusb.turnThruOff();    // turn off echo
  MIDIserial.turnThruOff(); // turn off echo

}

void loop() { 
  handleMIDIusb();
  handleMIDIserial();
  do_video();
}

//
void handleNoteOn(byte channel, byte note, byte vel) {
  Serial.printf("handleNoteOn! note:%d vel:%d\n", note, vel);
  digitalWrite(LED_BUILTIN,HIGH);
  // cancel attract mode
  if( millis() - last_millis_midi > millis_until_attract_mode ) { 
    for(int i=0; i<notes_cnt; i++) { 
      shapes_print(i); // debug
      shapes_shape_off(i, 0, 0, 0.5);
     }
  }  
  last_millis_midi = millis();
    
  // note stealing algorithm
  uint32_t oldest_note_millis = 0;
  int oldest_note = -1;  
  for( int i=0; i< notes_cnt; i++) { 
    if( notes_millis[i] == 0 ) {  // a free slot
      notes_on[i] = note;
      notes_millis[i] = millis();
      shapes_shape_on(i, note, vel, 1.001);
      oldest_note = -1;
      break;
    }
    else if( notes_millis[i] > oldest_note_millis ) { 
      oldest_note_millis = notes_millis[i];
      oldest_note = i;
    }
  }
  if( oldest_note != -1 ) {
    Serial.printf("using oldest note: %d\n", oldest_note);
    notes_on[oldest_note] = note;
    notes_millis[oldest_note] = millis();
    shapes_shape_on(oldest_note, note, vel, 1.001); // slow increase in size
  }
  
}

//
void handleNoteOff(byte channel, byte note, byte vel) {
  Serial.printf("handleNoteOff! note:%d vel:%d\n", note, vel);
  digitalWrite(LED_BUILTIN,LOW);
  last_millis_midi = millis();
  for( int i=0; i<notes_cnt; i++) { 
    if( notes_on[i] == note ) { 
      shapes_shape_off(i, note, vel, 0.99); // faster decrease in size
      notes_millis[i] = 0; // free up note slot
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
    dim_buf16(&display, fade_amount); // fade last frame
    shapes_update();
  }
  
  // attract mode and auto hue shift
  if( millis() - last_millis_hue > 100 ) { 
    last_millis_hue = millis();
    // attract mode if no MIDI
    if( millis() - last_millis_midi > millis_until_attract_mode ) { 
      shapes_attract();
    }
  }

  shapes_draw(&display);
  
}

//
void handleMIDIusb() {
  while( MIDIusb.read() ) {  // use while() to read all pending MIDI, shouldn't hang
    switch(MIDIusb.getType()) {
      case midi::ControlChange:
        //handleControlChange(0, MIDIusb.getData1(), MIDIusb.getData2());
        break;
      case midi::NoteOn:
        handleNoteOn( 0, MIDIusb.getData1(),MIDIusb.getData2());
        break;
      case midi::NoteOff:
        handleNoteOff( 0, MIDIusb.getData1(),MIDIusb.getData2());
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
        //handleControlChange(0, MIDIserial.getData1(), MIDIserial.getData2());
        break;
      case midi::NoteOn:
        handleNoteOn( 0, MIDIserial.getData1(),MIDIserial.getData2());
        break;
      case midi::NoteOff:
        handleNoteOff( 0, MIDIserial.getData1(),MIDIserial.getData2());
        break;
      default:
        break;
    }
  }
}
