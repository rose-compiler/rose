// Test case extracted from Hydro code
// using of array of classes or data structures
// Liao 7/6/2016
//

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

class Hydro 
{
  public: class VectorXY *bcVelocity;
// velocity BC values
  int *bc3Nodes;
// left boundary nodes
  int numBC3Nodes;
  void applyVelocityBC(class VectorXY *u);
}
;

void Hydro::applyVelocityBC(class VectorXY *u)
{
  if ((this) -> bcVelocity[3] . y != 0xdeadbeef) {
    for (int in = 0; in <= (this) -> numBC3Nodes - 1; in += 1) {
      u[(this) -> bc3Nodes[in]] . y = (this) -> bcVelocity[3] . y;
    }
  }
}
