#ifndef SHAPETHING_H
#define SHAPETHING_H

#include "picodvi_utils.h"

typedef struct ShapeThingStruct {
  int cx;  // center point of shape
  int cy;  // center point of shape
  int cr;  // size of shape things
  uint8_t hue;
  uint8_t sat;
  int8_t vhue;
  int rad;    // radius of little circles
  float ang;  // angle of first point on circle
  float vang; // how angle changes
  float vrad; // how radius changes
  bool visible;
} ShapeThing;

// initialize shapes
void shapes_init(ShapeThing shapes[], const int shapes_cnt) {
  for( int i=0; i<shapes_cnt; i++ ) {
    shapes[i].cx = 0;
    shapes[i].cy = 0;
    shapes[i].cr = 75;
    shapes[i].hue = 0;
    shapes[i].sat = 25;
    shapes[i].vhue = 0;
    shapes[i].rad = 2;
    shapes[i].ang = 0;
    shapes[i].vang = 0.01;
    shapes[i].vrad = 0.1;
    shapes[i].visible = false;
  }
}

// update the shape state every update period
void shapes_update(ShapeThing shapes[], const int shapes_cnt) {
  for( int i=0; i< shapes_cnt; i++) {
    //shapes[i].cr += shapes[i].vrad;
    shapes[i].ang += shapes[i].vang;
    shapes[i].hue += shapes[i].vhue; // maybe make these float?
  }
}

// draw all the shapes
void shapes_draw(DVIGFX16* disp, ShapeThing shapes[], const int shapes_cnt) {
  uint8_t r,g,b; // holders for hsbtorgb conversion

  //float f = millis() / 1000.0; // a sort of float framecount

  for( int i=0; i< shapes_cnt; i++ ) {
    if( !shapes[i].visible ) continue;
    int cx  = shapes[i].cx;
    int cy  = shapes[i].cy;
    float cr  = shapes[i].cr;
    int rad  = shapes[i].rad;
    float a1  = shapes[i].ang;

    hsbtorgb(&r, &g, &b,  shapes[i].hue, shapes[i].sat, 255);

    // modulate radius based on time and vrad
    cr = cr + cr * sin( millis() / shapes[i].vrad );

    int iters = 15;
    float da = TWO_PI/iters;

    for( int j=0; j<iters; j++ ) {
        float a = a1 + j*da;
        //disp->fillCircle( cx+cr*sin(a), cy+cr*cos(a), rad, disp->color565(r,g,b) );
        disp->drawFastHLine( cx+cr*sin(a), cy+cr*cos(a), rad, disp->color565(r,g,b) );
    }
  } // for shape

}

#endif
