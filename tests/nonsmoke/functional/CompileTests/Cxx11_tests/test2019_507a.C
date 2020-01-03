// #define B_FILE 1
// #include "defs.h"

/* _122Y24 expr a = f(a) requires temporary for the result of f(a) */
// #if (!defined(SKIP_122Y24)&&!defined(SKIP12)&&!defined(ONLY))||defined(CASE_122Y24)
#if 1
/* B-file */
// Implements core 320 - 2006 [editorial]
class X_
	{
public:
	int i;
	static int count;
     // X_(int ii) : i(ii) { ++count; }
     // X_(const X_ &x) { i = x.i; ++count; }
	};
#if 0
X_ f_(X_ x)
	{
	return x.i + 1;
	}
#endif
#endif /* CASE_122Y24 */

