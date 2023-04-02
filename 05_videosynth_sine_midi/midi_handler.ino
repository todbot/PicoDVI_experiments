
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

//// surely there's a way to have this generalized
//void handleMIDIserial() {
//  while( MIDIserial.read() ) {  // use while() to read all pending MIDI, shouldn't hang
//    switch(MIDIserial.getType()) {
//      case midi::ControlChange:
//        //handleControlChange(0, MIDIserial.getData1(), MIDIserial.getData2());
//        break;
//      case midi::NoteOn:
//        //handleNoteOn( 0, MIDIserial.getData1(),MIDIserial.getData2());
//        break;
//      case midi::NoteOff:
//        //handleNoteOff( 0, MIDIserial.getData1(),MIDIserial.getData2());
//        break;
//      default:
//        break;
//    }
//  }
//}
//
//
////
//void handleNoteOn(byte channel, byte note, byte velocity) {
//  Serial.println("midi_test handleNoteOn!");
//  digitalWrite(LED_BUILTIN,HIGH);
//}
//
////
//void handleNoteOff(byte channel, byte note, byte velocity) {
//  digitalWrite(LED_BUILTIN,LOW);
//  envelope.noteOff();
//}
//
////
//void handleControlChange(byte channel, byte cc_num, byte cc_val) {
//  #if DEBUG_MIDI 
//  Serial.printf("CC %d %d\n", cc_num, cc_val);
//  #endif
//  for( int i=0; i<CC_COUNT; i++) { 
//    if( midi_ccs[i] == cc_num ) { // we got one
//      mod_vals[i] = cc_val;
//      // special cases, not set every updateControl()
//      if( i == PortamentoTime ) { 
//      }
//      else if( i == EnvReleaseTime ) {
//         envelope.setReleaseTime( mod_vals[EnvReleaseTime]*10 );
//      }
//    }
//  }
//}
