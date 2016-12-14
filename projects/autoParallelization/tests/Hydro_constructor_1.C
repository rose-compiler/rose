// HydroC
// Symptom: VectorXY::VectorXY() side effects are unknown for the loop statements
// Real solution needs interprocedural side effect analysis
// A workaround is to pass annotation files for function side effect analysis
// ../annot/tinyHydro.annot
class VectorXY
{
  public:
    VectorXY(){x=0.0; y=0.0;}
    VectorXY(double xx, double yy){x=xx; y=yy;}

    double x;
    double y;

};

#if 1
class Hydro
{
  public:
    Hydro(int c);
    VectorXY * bcVelocity; // velocity BC values
    int cycle;
};

Hydro::Hydro (int c): cycle(c)
{
   for (int i = 0; i < 4; i ++)
      bcVelocity[i] = VectorXY(0xdeadbeef, 0xdeadbeef);
}

#endif
void foo()
{
  VectorXY * bcVelocity; 
  for (int i = 0; i < 4; i ++)
    bcVelocity[i] = VectorXY(0xdeadbeef, 0xdeadbeef);
}

