#include "rose.h"
#include <math.h>

void
heatmap(float f, uint8_t& r, uint8_t& g, uint8_t& b)
   {
     double ff = f;
     if (false && ff <= 4.0)
        {
          r = g = b = 0;
        }
       else
        {
          ff = .5 * log(ff + 1) - 0.0;
          if (ff < 0) ff = 0;
          int ffint = (int)floor(ff);
          double ffrest = ff - floor(ff);
          uint8_t c = ffrest < 0 ? 0 : ffrest > 1 ? 255 : (uint8_t)(255 * ffrest);

          switch (ffint)
             {
               case 0: // black -> blue
                    r = g = 0; b = c;
                    break;
               case 1: // blue -> cyan -> green
                    r = 0; g = (c <= 127 ? c * 2 : 255); b = (c >= 129 ? 255 - 2 * (c - 129) : 255);
                    break;
               case 2: // green -> yellow
                    r = c; g = 255; b = 0;
                    break;
               case 3: // yellow -> red
                    r = 255; g = 255 - c; b = 0;
                    break;
               case 4: // red -> magenta
                    r = 255; g = 0; b = c;
                    break;
               case 5: // magenta -> white
                    r = 255; g = c; b = 255;
                    break;
               default: // white
                    r = g = b = 255;
             }
        }
   }

