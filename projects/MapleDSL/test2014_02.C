#include "MapleDSL.h"

int main()
   {
  // Discretization specification.

  // First-order forward and backward distretization operators (primatives) for the X-axis
     D_plus dpx(0);
     D_minus dmx(0);

  // Build a second order operator (X-axis)
  // Operator DPDMx;
  // DPDMx = dpx(dmx);
     Operator DPDMx = dpx(dmx);

#if 0
  // First-order forward and backward distretization operators (primatives) for the Y-axis
     D_plus dpy(1);
     D_minus dmy(1);

  // Build a second order operator (Y-axis)
     Operator DPDMy;
     DPDMy = dpy(dmy);

  // Build the multidimensional (2D) second order operator (X-axis and Y-axis)
     Operator Laplace2D = DPDMx + DPDMy;
#else
  // Build the multidimensional (2D) second order operator (X-axis and Y-axis)
  // Operator Laplace2D = DPDMx;
#endif

#if 0
  // Build data on which to apply operator.
     GridFunction u,v;

  // Initialize the data
     u = 0.0;

  // Application of stencil on u saved in v.
     v = Laplace2D(u);

  // Output data in v.
     v.print();
#endif
   } 

