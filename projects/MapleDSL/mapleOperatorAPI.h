
class StencilPoint
   {
     public:
       // We need to know when the coefficientValue are zero (assume 
       // equality operators are a bad idea on floating point values).
          bool isNonzero;

       // This is the non-zero value of the stincil coefficient.
          double coefficientValue;

          StencilPoint();
          StencilPoint(bool temp_isNonzero, double temp_coefficientValue);

         ~StencilPoint();
   };

class StencilOperator
   {
  // In contrast to the Shift Calculus, this is a dense multi-dimensional 
  // array of stencil coefficient, with explicit information about when 
  // they are non-zero values.

     public:
       // We only want to handle non-operator weighted stencil with this data structure.
          bool isOperatorWieghtedStencil;

       // Representation of a multi-dimensional array of stencil points.
       // I am supporting only 2D stencils while I debug this.
          typedef std::vector< std::vector< std::vector< StencilPoint > > > StencilType;

          StencilType stencil;

          StencilOperator(int size_x, int size_y = 1, int size_z = 1);

          StencilPoint & operator()(int i, int j, int k);

         ~StencilOperator();

   };
