
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

const int shape_cnt = 6; 

ShapeThing shapes[shape_cnt];

float phase = 0;
float phase_inc = 0.001;

// just like map() but works with floats
inline float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// initialize shapes
void shapes_init() {
  for( int i=0; i<shape_cnt; i++ ) { 
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
void shapes_shape_on(uint8_t i, uint8_t note, uint8_t vel) {
  shapes[i].cx = dw/4 + (note%12) * 20;
  shapes[i].hue = random(64) + (note%12) * 127/12;
  shapes[i].sat = random(127,255);
  shapes[i].p1 = random(3,7);
  shapes[i].p2 = random(4,9);
  shapes[i].r0 = 1.0;
  shapes[i].r0_decay = 1;
}

// turn a shape off 
void shapes_shape_off(uint8_t i, uint8_t note, uint8_t vel) {
  shapes[i].r0_decay = 0.9;  
}

// update the shape state every update period
void shapes_update() { 
  for( int i=0; i< shape_cnt; i++) { 
    shapes[i].r0 = shapes[i].r0 * shapes[i].r0_decay;   // decay size over time
    shapes[i].pv = shapes[i].pv + 0.2;
  }
}

// attract mode
void shapes_attract() {
  float decays[] = { 0.996, 1.01, 0.93, 1.005 };
  for( int i=0; i< shape_cnt; i++) { 
    shapes[i].hue++;
    if( shapes[i].r0 < 0.2 || shapes[i].r0 > 1.5 ) { 
      shapes_shape_on(i, random(20,80), random(127,255) );
      shapes[i].r0 = 1;
      shapes[i].r0_decay = decays[ random(4) ];
    }    
  }
}

// draw all the shapes
void shapes_draw(GFXcanvas16* disp) {
  uint8_t r,g,b; // holders for hsbtorgb conversion
  
  float f = millis() / 1000.0; // a sort of float framecount

  for( int i=0; i< shape_cnt; i++ ) {
    
    float cx = shapes[i].cx;
    float cy = shapes[i].cy;
    float r0 = shapes[i].r0;
    float p1 = shapes[i].p1;
    float p2 = shapes[i].p2;
    float pv = shapes[i].pv;
    
    float r1Min = mapf(sin(f), -1, 1,  20, 70);
    float r1Max = mapf(sin(f/2), -1, 1, 0, 50);
    float r2Min = mapf(sin(f/2), -1, 1,  20, 50);
    float r2Max = mapf(sin(f/2.3), -1, 1, 0, 50);
  
    hsbtorgb(&r, &g, &b,  shapes[i].hue, shapes[i].sat, 255);

    float r1, r2, br;
    
    for(float a = 0; a < TWO_PI; a += 0.05 ) { 
      r1 = mapf(sin(a * p1), -1, 1, r1Min, r1Max);
      r2 = mapf(sin(a * p2 + pv), -1, 1, r2Min, r2Max);
      br = r0 * (r1 + r2) ; 
      float x = br * cos(a);
      float y = br * sin(a);
      // "circle" is a plus-sign when r=1 haha
      disp->fillCircle( cx + x, cy + y, 1, display.color565(r,g,b) );
    } // for angle
    
  } // for shape
  
  phase += phase_inc;

}
