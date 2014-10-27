#include "Maple.h"

void foo()
   {
  // This is a less importatn example.
     D_plus d1(0);
     D_plus d2(0);

     D_plus d3(0);

     d1 = d2 + d3;

     d1(d2);

     d1(d2);
      
     Operator X = d1(d2) + d1(d3);

     GridFunction u,v;

  // Application of stencil on u saved in v.
     v = X(u);
   } 

void foo2()
   {
     D_plus dpx(0);
     D_minus dmx(0);

     Operator DPDMx;
     DPDMx = dpx(dmx);
      
     D_plus dpy(1);
     D_minus dmy(1);

     Operator DPDMy;
     DPDMy = dpy(dmy);

     Operator Lap2D = DPDMx + DPDMy;

     GridFunction u,v;

  // Application of stencil on u saved in v.
     v = Lap2D(u);
   } 

