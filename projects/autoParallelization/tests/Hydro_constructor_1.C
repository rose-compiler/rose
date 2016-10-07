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

/*

Assuming all arrays used as indirect indices have unique elements (no overlapping) ...

Unparallelizable loop at line:24 due to the following dependencies:
0x7fff2904c960 Distance Matrix size:1*1 IO_DEP; commonlevel = 1 CarryLevel = (0,0)  Not precise
        SgExprStatement:(this) -> bcVelocity[i] = VectorXY(0xdeadbeef,0xdeadbeef);@25:7->
        SgExprStatement:(this) -> bcVelocity[i] = VectorXY(0xdeadbeef,0xdeadbeef);@25:7
* 0;||::

Unparallelizable loop at line:31 due to the following dependencies:
0x7fff2904c960 Distance Matrix size:1*1 IO_DEP; commonlevel = 1 CarryLevel = (0,0)  Not precise
        SgExprStatement:bcVelocity[i] = VectorXY(0xdeadbeef,0xdeadbeef);@32:5->
        SgExprStatement:bcVelocity[i] = VectorXY(0xdeadbeef,0xdeadbeef);@32:5
* 0;||::

 * */
