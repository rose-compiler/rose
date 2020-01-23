// #if (!defined(SKIP_122Y24)&&!defined(SKIP12)&&!defined(ONLY))||defined(CASE_122Y24)
#if 1
class X_
	{
public:
	int i;
	static int count;
     // X_(int ii) : i(ii) { ++count; }
     // X_(const X_ &x) { i = x.i; ++count; }
	};
int X_::count = 0;
X_ f_(X_ x);
#endif /* CASE_122Y24 */

// #include "final_defs.h"
int main(int argc, char *argv[])
{
#if 0
  // Implements core 320 - 2006 [editorial]
     X_ a(1);
  // ieq(X_::count, 1);
     a = f_(a);
  // chk(X_::count >= 3);
#endif
}
