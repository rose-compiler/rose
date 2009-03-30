#define PI 3.14159265358979323846
#define fixangle(a) ((a) - 360.0 * (floor((a) / 360.0)))  /* Fix angle	  */

#define torad(d) ((d) * (PI / 180.0))			  /* Deg->Rad	  */
#include <math.h>

int main()
{
  double A4, MmP, lP, Lambdasun;
  torad(2 * MmP);
  torad(2 * (lP - Lambdasun));
  torad(MmP);
  A4 = 0.214 * sin(torad(2 * MmP));
  A4 = 0.6583 * sin(torad(2 * (lP - Lambdasun)));
  Lambdasun = fixangle(MmP + Lambdasun); 
  A4 = fixangle(Lambdasun - 0.1114041 * A4 - Lambdasun);

};
