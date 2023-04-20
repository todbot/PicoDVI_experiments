#ifndef SHAPETHING_H
#define SHAPETHING_H

#include "picodvi_utils.h"

typedef struct ShapeThingStruct {
    int cx;
    int cy;
    uint8_t hue;
    uint8_t sat;
    int8_t vhue;
    float rad; // radius of circle points are on
    float ang; // angle of first point on circle
    float vrad; // how radius changes
    float vang; // how angle changes
    bool visible;
} ShapeThing;

// initialize shapes
void shapes_init(ShapeThing shapes[], const int shapes_cnt, const int cx, const int cy) {
  for( int i=0; i<shapes_cnt; i++ ) { 
    shapes[i].cx = cx; 
    shapes[i].cy = cy;
    shapes[i].hue = 0;
    shapes[i].sat = 255;
    shapes[i].vhue = 0;
    shapes[i].rad = 100;
    shapes[i].ang = 0;
    shapes[i].vrad = 0;
    shapes[i].vang = 0.01;
    shapes[i].visible = false;
  }
}

// update the shape state every update period
void shapes_update(ShapeThing shapes[], const int shapes_cnt) {
  for( int i=0; i< shapes_cnt; i++) { 
    shapes[i].rad += shapes[i].vrad;
    shapes[i].ang += shapes[i].vang;
    shapes[i].hue += shapes[i].vhue; // maybe make these float?
  }
}

//// draw all the shapes
void shapes_draw(DVIGFX16* disp, ShapeThing shapes[], const int shapes_cnt) {
  uint8_t r,g,b; // holders for hsbtorgb conversion
  
  //float f = millis() / 1000.0; // a sort of float framecount

  for( int i=0; i< shapes_cnt; i++ ) {
    if( !shapes[i].visible ) continue;
    float cx  = shapes[i].cx;
    float cy  = shapes[i].cy;
    float rad = shapes[i].rad;
    float a1  = shapes[i].ang;
    float a2  = a1 + 1*(TWO_PI/3);
    float a3  = a1 + 2*(TWO_PI/3);
       
    hsbtorgb(&r, &g, &b,  shapes[i].hue, shapes[i].sat, 255);

    disp->drawTriangle( cx+rad*sin(a1), cy+rad*cos(a1),
                        cx+rad*sin(a2), cy+rad*cos(a2),
                        cx+rad*sin(a3), cy+rad*cos(a3),
                        disp->color565(r,g,b) );
  } // for shape
  
}
//
////// attract mode
////void shapes_attract() {
////  float decays[] = { 0.996, 1.01, 0.93, 1.005 };
////  for( int i=0; i< shapes_cnt; i++) { 
////    shapes[i].hue++;
////    // make a new shape if old one gone
////    if( shapes[i].r0 < 0.2 || shapes[i].r0 > 1.5 ) {
////      shapes_shape_on(i, random(20,80), random(127,255), decays[ random(4) ] );
////    }    
////  }
////}
//
//
//void shapes_print(int i) { 
////  Serial.printf("%d: r0_decay:%1.2f\n", i, shapes[i].r0_decay);
//}

#endif
