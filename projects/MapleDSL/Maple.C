#include "Maple.h"

void foo()
   {
     D_plus dpx(0);
     D_minus dmx(0);

     Operator DPDMx;

     double a1 = 2.0;
     double a2 = 3.0;

  // Different ways of forming first-order operators (with coefficient values).
     DPDMx = dpx(dmx);
     DPDMx = a1 * dpx(a2 * dmx);
     DPDMx = dpx(a2 * dmx);
     DPDMx = a1 * dpx(dmx);

  // Different ways of forming first-order operators with explicit specification of target grid function (and with coefficient values).
     GridFunction u;
     DPDMx = dpx(dmx(u));
     DPDMx = dpx(dmx(a1*u));

     D_plus dpy(1);
     D_minus dmy(1);

     Operator DPDMy;
     DPDMy = dpy(dmy);

#if 1
  // Example of specification of PDE not relative to a grid function.
     Operator Lap2D_0 = DPDMx + DPDMy;
#endif
#if 1
  // Example of specification of linear PDE with explicit grid function "u".
     Operator Lap2D_1 = dpx(dmx(u)) + dpy(dmy(u));
#endif
#if 1
  // Example of specification of linear PDE with explicit grid function "u" and explicit constants.
     double a_value,b_value;
     Operator Lap2D_2 = dpx(dmx(a_value*u)) + dpy(dmy(b_value*u));
#endif
#if 1
  // Example of specification of linear PDE.
     CoefficientFunction a_linear_coefficient_function;
     CoefficientFunction b_linear_coefficient_function;
     Operator Lap2D_3 = dpx(dmx(a_linear_coefficient_function()*u)) + dpy(dmy(b_linear_coefficient_function()*u));
#endif
#if 1
  // Example of specification of nonlinear PDE.
  // Demonstration of coefficient being a non-linear operator on GridFunction u (we could use an alternative type than GridFunction).
     CoefficientFunction a_nonlinear_coefficient_function(const GridFunction & u);
     CoefficientFunction b_nonlinear_coefficient_function(const GridFunction & u);

     Operator Lap2D_4 = dpx(dmx(a_nonlinear_coefficient_function(u)*u)) + dpy(dmy(b_nonlinear_coefficient_function(u)*u));
#endif

#if 1
  // "c" is the wave speed.
     double delta_t, delta_x, c;
  // The operator returns u and v at the new time level.
     Operator Wave_v_1 = v + delta_t * c*c * dpx(dmx(u)) + (delta_t/2)* c * delta_x * dpx(dmx(v));
     Operator Wave_u_1 = u + delta_t * (v + delta_t/2 * c*c * dpx(dmx(u)) + (delta_t/4)* c * delta_x * dpx(dmx(v)));
#endif

     GridFunction u_input,v_output;

  // Application of stencil on u saved in v.
     v_output = Lap2D_1(u_input);
   } 

