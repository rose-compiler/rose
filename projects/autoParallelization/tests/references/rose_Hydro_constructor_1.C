// HydroC
// Symptom: VectorXY::VectorXY() side effects are unknown for the loop statements
// Real solution needs interprocedural side effect analysis
// A workaround is to pass annotation files for function side effect analysis
// ../annot/tinyHydro.annot
#include <omp.h> 

class VectorXY 
{
  

  public: inline VectorXY()
{
    (this) -> x = 0.0;
    (this) -> y = 0.0;
  }
  

  inline VectorXY(double xx,double yy)
{
    (this) -> x = xx;
    (this) -> y = yy;
  }
  double x;
  double y;
}
;
#if 1

class Hydro 
{
  public: Hydro(int c);
  class VectorXY *bcVelocity;
// velocity BC values
  int cycle;
}
;

Hydro::Hydro(int c) : cycle(c)
{
  
#pragma omp parallel for
  for (int i = 0; i <= 3; i += 1) {
    (this) -> bcVelocity[i] = VectorXY(0xdeadbeef,0xdeadbeef);
  }
}
#endif

void foo()
{
  class VectorXY *bcVelocity;
  
#pragma omp parallel for
  for (int i = 0; i <= 3; i += 1) {
    bcVelocity[i] = VectorXY(0xdeadbeef,0xdeadbeef);
  }
}
