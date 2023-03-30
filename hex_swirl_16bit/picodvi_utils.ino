// GFX_utils for Adafruit_GFX and PicoDVI
// 29 Mar 2023 - @todbot / Tod Kurt
//
#include <PicoDVI.h>

// convert 8-bit (0-255) R,G,B to a 16-bit RGB565 color value
inline uint16_t RGBtoRGB565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}
// convert 16-bit RGB565 color to 8-bit (0-255) R,G,B values
inline void RGB565toRGB(uint8_t* r, uint8_t* g, uint8_t* b, uint16_t color565) {
  *b = ((color565 & 0x001F)>> 0) *8;    // 5 bits blue
  *g = ((color565 & 0x07E0)>> 5) *4;    // 6 bits green
  *r = ((color565 & 0xF800)>>11) *8;    // 5 bits red
}

// "dim" entire display buffer by amount 'n'
// handling RGB color correctly 
void dim_buf16(DVIGFX16* disp, int n) {
  uint16_t* buf = disp->getBuffer();
  uint8_t r,g,b;
  for( int i=0; i< dw*dh; i++) {
     RGB565toRGB(&r,&g,&b, buf[i]);
     r = max(r-n,0);
     g = max(g-n,0);
     b = max(b-n,0);
     buf[i] = RGBtoRGB565(r,g,b);
  }
}

// "dim" entire buffer by amount n,
// (but it's palette so does gradient)
void dim_buf8(DVIGFX8* disp, int n) {
  uint8_t* buf = disp->getBuffer();
  for( int i=0; i< dw*dh; i++) {
    buf[i] = max(buf[i]-n,0);
  }
}


// make rainbow palette
void make_rainbow_palette(DVIGFX8* disp) {
  uint8_t r,g,b;
  for( int i=1; i<254; i++) {     
    hsbtorgb(&r, &g, &b,  i, 255,255);
    Serial.printf("i:%d %3d %3d %3d\n", i, r,g,b);
    disp->setColor(i, r,g,b);
  }
  disp->setColor(0, 0x0000); // first palette entry = black
  disp->setColor(255, 0xFFFF); // Last palette entry = white
}

// make the "websafe colors" palette
void make_websafe_palette(DVIGFX8* disp) { 
  uint8_t i=0;
  for( int r=0; r<16; r+=3 ) 
    for( int g=0; g<16; g+=3 )  
      for( int b=0; b<16; b+=3 ) 
        disp->setColor(i++, r*16, g*16, b*16);     
}

// make R,G,B gradient palette
void make_rgb_palette(DVIGFX8* disp) { 
  uint8_t i=0;
  for(int r=0; r<64; r++) { disp->setColor(i++, r*4, 0,0);  }
  for(int g=0; g<64; g++) { disp->setColor(i++, 0, g*4, 0); }
  for(int b=0; b<64; b++) { disp->setColor(i++, 0, 0, b*4); }
  for(int w=0; w<64; w++) { disp->setColor(i++, w*4, w*4, w*4); }
}

// make a N-color gradient to black palette, with a N equally-spaced colors
// based off of a starting hue
void make_random_ncolor_palette(DVIGFX8* disp, uint8_t hue_start, int num_colors) { 
  uint8_t dh = 256 / num_colors; // == 64 for 4 colors
  uint8_t i=0;
  uint8_t r,g,b;
  for( int j = 0; j < num_colors; j++) {
    uint8_t h = hue_start + j*dh;  // space equally around colorwheel
    for( int k=0; k < dh; k++) { 
      hsbtorgb(&r, &g, &b,  h, 255, k*num_colors);
      disp->setColor(i++, r,g,b);
    }
  }
}

// integer-only hsbtorgb
// from blink1-lib: https://github.com/todbot/blink1-tool/blob/main/blink1-lib.c
// orig from: http://web.mit.edu/storborg/Public/hsvtorgb.c
void hsbtorgb( uint8_t* r, uint8_t* g, uint8_t* b,  uint8_t h, uint8_t s, uint8_t v )
{
    unsigned char region, fpart, p, q, t;

    if(s == 0) {          // color is grayscale
        *r = *g = *b = v;
        return;
    }
    
    region = h / 43;      // make hue 0-5
    fpart = (h - (region * 43)) * 6; // find remainder part, make it from 0-255

    // calculate temp vars, doing integer multiplication
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * fpart) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;

    // assign temp vars based on color cone region
    switch(region) {
        case 0:   *r = v; *g = t; *b = p; break;
        case 1:   *r = q; *g = v; *b = p; break;
        case 2:   *r = p; *g = v; *b = t; break;
        case 3:   *r = p; *g = q; *b = v; break;
        case 4:   *r = t; *g = p; *b = v; break;
        default:  *r = v; *g = p; *b = q; break;
    }
}
