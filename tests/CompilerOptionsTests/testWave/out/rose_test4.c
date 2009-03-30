#define	PI	3.14159265358979323846
#define	fixangle(a)	((a) - 360.0 * (floor((a) / 360.0)))
#define	torad(d)	((d) * (PI / 180.0))
#include <math.h>

int main()
{
  double A4;
  double MmP;
  double lP;
  double Lambdasun;
  ((2) * MmP) * (3.14159265358979323846 / 180.0);
  ((2) * (lP - Lambdasun)) * (3.14159265358979323846 / 180.0);
  MmP * (3.14159265358979323846 / 180.0);
  A4 = (0.214 * sin((((2) * MmP) * (3.14159265358979323846 / 180.0))));
  A4 = (0.6583 * sin((((2) * (lP - Lambdasun)) * (3.14159265358979323846 / 180.0))));
  Lambdasun = ((MmP + Lambdasun) - (360.0 * floor(((MmP + Lambdasun) / 360.0))));
  A4 = (((Lambdasun - (0.1114041 * A4)) - Lambdasun) - (360.0 * floor((((Lambdasun - (0.1114041 * A4)) - Lambdasun) / 360.0))));
  return 0;
}

