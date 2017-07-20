#if 1
// read: a
// write: b
void bar (int a, int& b)
{
  b = a; 
}
#endif


#if 1
double gx;

// read: empty
// write: gx
void globalX()
{
  gx=0.0; 
}
#endif

#if 1
class VectorXY
{
  public:
    VectorXY() {x=0.0; y=0.0;}
    VectorXY(double xx, double yy) {x=xx; y=yy;}  // side effect should be obvious

    double x;
    double y;
};
#endif

#if 1
void foo()
{
  VectorXY * bcVelocity;
  for (int i = 0; i < 4; i ++)
    bcVelocity[i] = VectorXY(0xdeadbeef, 0xdeadbeef);   // VectorXY::VectorXY () side effect unknown, even the source code is available. 
}
#endif

#if 0
void foo2()
{
  VectorXY * bcVelocity;
  double d1=0xdeadbeef, d2=0xdeadbeef; 
  for (int i = 0; i < 4; i ++)
    bcVelocity[i] = VectorXY(d1, d2);   // VectorXY::VectorXY () side effect unknown, even the source code is available. 
}
#endif
