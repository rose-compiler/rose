
class DerivativeApproximation
{
  private:
  template <int dim, class InputVector>
  void
  approximate_gradient (const InputVector     &solution);

};

template <int dim, class InputVector>
void 
DerivativeApproximation::
approximate_gradient (const InputVector     &solution)
{
}

template void DerivativeApproximation::
approximate_gradient<1,double>(const double &solution);
