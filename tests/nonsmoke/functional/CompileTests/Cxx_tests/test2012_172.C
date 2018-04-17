// This is part of test2007_38.C (and simpler, I hope).

class DerivativeApproximation
   {
     private:
          template <int dim> void approximate_gradient();
   };

template <int dim>
void DerivativeApproximation::approximate_gradient() {}

template void DerivativeApproximation::approximate_gradient<1>();
