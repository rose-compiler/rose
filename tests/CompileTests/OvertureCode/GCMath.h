#ifndef GCMath_

//
// Who to blame:  Geoff Chesshire
//

#include <math.h>
#include "OvertureTypes.h"

inline Integer min0 (const Integer& i, const Integer& j)
  { return i < j ? i : j; }
inline Integer max0 (const Integer& i, const Integer& j)
  { return i > j ? i : j; }
inline Real amin1(const Real& x, const Real& y) { return x < y ? x : y; }
inline Real amax1(const Real& x, const Real& y) { return x > y ? x : y; }
inline Integer Iabs (const Integer& i )         { return i < 0 ? -i : i; }
inline Real Fabs (const Real& x )               { return x < 0 ? -x : x; }
inline Real Sqr(const Real& x)                  { return x * x; }
inline Real Sqrt(const Real& x)                 { return (Real)sqrt(x); }
inline Integer Isign(const Integer& i, const Integer& j)
  { return j < 0 ? -Iabs(i) : Iabs(i); }
inline Real Fsign(const Real& x, const Real& y)
  { return y < 0 ? -Fabs(x) : Fabs(x); }

#define GCMath_
#endif // GCMath_
