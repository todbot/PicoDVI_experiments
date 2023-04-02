
typedef struct ShapeThingStruct {
    int cx;
    int cy;
    uint8_t hue;
    uint8_t sat;
    int p1;
    int p2;
    float pv;
    float r0;
    float r0_decay;
} ShapeThing;

const int shapes_cnt = 8; // FIXME: must match notes_cnt
ShapeThing shapes[shapes_cnt];

float phase = 0;
float phase_inc = 0.001;
float pv_mod = 0.2;
int hue_mod = 0;

// just like map() but works with floats
inline float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// initialize shapes
void shapes_init() {
  for( int i=0; i<shapes_cnt; i++ ) { 
    shapes[i].cx = dw/2; 
    shapes[i].cy = dh/2;
    shapes[i].hue = 0;
    shapes[i].sat = 255;
    shapes[i].p1 = 5;
    shapes[i].p2 = 6;
    shapes[i].pv = 0;
    shapes[i].r0 = 0.1;
    shapes[i].r0_decay = 1;
  }
}

// turn a shape on (like on note_on)
void shapes_shape_on(uint8_t i, uint8_t note, uint8_t vel, float r0decay) {
  shapes[i].cx = dw/5 + (note%12) * 20;
  shapes[i].hue = map(note, 30,100, 0, 230);
  shapes[i].sat = random(127,255);
  shapes[i].p1 = random(3,8);
  shapes[i].p2 = random(4,9);
  shapes[i].r0 = 0.5 + (1-(note/127.0));
  shapes[i].r0_decay = r0decay;
}

// turn a shape off 
void shapes_shape_off(uint8_t i, uint8_t note, uint8_t vel, float r0decay) {
  shapes[i].r0_decay = r0decay;  
}

// update the shape state every update period
void shapes_update() {
  for( int i=0; i< shapes_cnt; i++) { 
    shapes[i].r0 = shapes[i].r0 * shapes[i].r0_decay; // decay size over time
    shapes[i].pv = shapes[i].pv + pv_mod; //
    shapes[i].hue += hue_mod;  // hack
  }
}

void shapes_set_pvmod(float pvmod) { 
//  for( int i=0; i< shapes_cnt; i++) { 
//    shapes[i].hue += (pvmod*10); // hack
//  }  
  pv_mod = pvmod;
  hue_mod = pvmod * 10; // pvmod ranges 0.0 - 1.0
}

void shapes_print(int i) { 
  Serial.printf("%d: r0_decay:%1.2f\n", i, shapes[i].r0_decay);
}

// attract mode
void shapes_attract() {
  float decays[] = { 0.996, 1.01, 0.93, 1.005 };
  for( int i=0; i< shapes_cnt; i++) { 
    shapes[i].hue++;
    // make a new shape if old one gone
    if( shapes[i].r0 < 0.2 || shapes[i].r0 > 1.5 ) {
      shapes_shape_on(i, random(20,80), random(127,255), decays[ random(4) ] );
    }    
  }
}

// draw all the shapes
void shapes_draw(GFXcanvas16* disp) {
  uint8_t r,g,b; // holders for hsbtorgb conversion
  
  float f = millis() / 1000.0; // a sort of float framecount

  for( int i=0; i< shapes_cnt; i++ ) {
   
    float cx = shapes[i].cx;
    float cy = shapes[i].cy;
    float r0 = shapes[i].r0;
    float p1 = shapes[i].p1;
    float p2 = shapes[i].p2;
    float pv = shapes[i].pv;

    if( r0 < 0.2 ) { continue; } // don't bother drawing the little ones
    
    float r1Min = mapf(sin(f), -1, 1,  20, 70);
    float r1Max = mapf(sin(f/2), -1, 1, 0, 50);
    float r2Min = mapf(sin(f/2), -1, 1,  20, 50);
    float r2Max = mapf(sin(f/2.3), -1, 1, 0, 50);
    
    hsbtorgb(&r, &g, &b,  shapes[i].hue, shapes[i].sat, 255);

    float r1, r2, br;
    const float ainc = 0.05; // degrees(0.5) =~ 0.087
    int16_t last_x=0, last_y=0, new_x, new_y;
    for(float a = 0; a < TWO_PI+ainc; a += ainc ) { 
      r1 = mapf(sin(a * p1), -1, 1, r1Min, r1Max);
      r2 = mapf(sin(a * p2 + pv), -1, 1, r2Min, r2Max);
      br = r0 * (r1 + r2) ; 
      float x = br * cos(a);
      float y = br * sin(a);
      // "circle" is a plus-sign when r=1 haha
      // in total these fillCircle() calls take 20millis for 8 shapes at ainc = 0.05
      //disp->fillCircle( cx + x, cy + y, 1, display.color565(r,g,b) );
      // and drawFastVLine() & drawFastHLine() takes 18 & 19 millis
      //disp->drawFastHLine( cx + x, cy + y, 2, display.color565(r,g,b) );
      disp->drawFastVLine( cx + x, cy + y, 2, display.color565(r,g,b) );
      
//      // 11 millis by reducing ainc to 0.1 and drawing connected lines 
//      new_x = cx+x; new_y = cy+y;
//      if( last_x==0 ) { last_x = new_x; last_y = new_y; }
//      disp->drawLine( last_x, last_y, new_x, new_y, display.color565(r,g,b) );
//      last_x = new_x; last_y = new_y;
      
    } // for angle
    
  } // for shape
  
  phase += phase_inc;

}
