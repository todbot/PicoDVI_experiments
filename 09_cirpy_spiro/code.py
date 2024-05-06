# cirpy_spiro_dvi_code.py -- PicoDVI test on Feather RP2040
# 6 May 2024 - @todbot / Tod Kurt
# demo at:

import time, math, random
import board
import displayio
import bitmaptools

display = board.DISPLAY
display.auto_refresh = False
dw,dh = display.width, display.height
cx,cy = dw//2, dh//2
display.root_group = displayio.Group()

# create a palette of colors to choose from, this one is shades of neon green
num_shades = 16
pal = displayio.Palette(num_shades)
for i in range(len(pal)):
    r = 0
    g = int(255 / len(pal) * i)
    b = int(100 / len(pal) * i)
    pal[i] = r<<16 | g<<8 | b

bitmap = displayio.Bitmap(dw, dh, num_shades)
tile_grid = displayio.TileGrid(bitmap, pixel_shader=pal)
display.root_group.append(tile_grid)

def make_spirograph(a,b,h, dt, colr):
    """ from: https://samjbrenner.com/notes/processing-spirograph/"""
    t = 0
    ot = dt
    od = a*ot
    ab = a-b
    oxpos = int( ab*math.cos(ot) + h*math.cos(od))
    oypos = int( ab*math.sin(ot) + h*math.sin(od))
    while t < 6.28:
        d = a*t
        xpos = int( ab*math.cos(t) + h*math.cos(d))
        ypos = int( ab*math.sin(t) + h*math.sin(d))
        bitmaptools.draw_line(bitmap, cx+oxpos, cy+oypos, cx+xpos, cy+ypos, colr)
        ot, od = t, d  # save last angle and positions
        oxpos, oypos = xpos, ypos
        t += dt

a0 = 20  # spirograph parameter
b = 55  # spirograph parameter
h0 = 60  # spirograph parameter

last_time = 0
tic = 100  # position in phase space, arbitrary starting point
colr = 0
colri = 1

while True:
    tic += 0.01
    a = a0 + 10 * math.sin(tic/5)  # evolve our spirograph settings
    h = h0 + 10 * math.sin(tic*3)   # evolve our spirograph settings
    print("%.2f spiro! %3.2f %.2f, %.2f, %.2f" %(time.monotonic(), tic, a,b,h))
    #colr = random.randint(0,num_shades-1)
    #colr = (colr+1) % num_shades
    colr = colr + colri
    if colr == 0 or colr == num_shades-1: colri = -colri

    make_spirograph( a, b, h, dt=0.03, colr=colr )
    
    if time.monotonic() - last_time > 0.1:
        last_time = time.monotonic()
        # drat can't do this
        #tile_grid.bitmap = bitmapfilter.mix(bitmap, dim) 
    display.refresh()
