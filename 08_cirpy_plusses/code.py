# cirpy_plusses_code.py -- PicoDVI test on Feather RP2040
# 26 Apr 2023 - @todbot / Tod Kurt
# demo at: https://www.youtube.com/live/Gcw8rOYaO8U?feature=share&t=893

import time, random
import board, displayio, vectorio

display = board.DISPLAY
print("display width, height:", display.width, display.height)

maingroup = displayio.Group()
display.root_group = maingroup
pal = displayio.Palette(2)
pal[0] = 0x00ffff
pal[1] = 0xffff00

wcnt, hcnt = 32, 24  # if we're actually 320x240, this gives us 10 pixel spacing
for i in range(wcnt):
    for j in range(hcnt):
        x,y = 5 + i * 10, 5 + j * 10  # 10 pixel spacing
        c = vectorio.Circle(pixel_shader=pal, color_index=(i%2), radius=5, x=x, y=y)
        maingroup.append(c)

while True:
    #print("hi")
    time.sleep(0.01)
    n = random.randint(0,len(display.root_group)-1)
    display.root_group[n].hidden = not display.root_group[n].hidden

