
int operator "" _LL(unsigned long long) { return 1; }

#if 0
int operator "" _CCS(const char*, unsigned long) { return 2; }

static int operator "" _LD(long double) { return 3; }

namespace ud_ops 
   {
     int operator "" _C(char) { return 4; }
     int operator "" _C(wchar_t) { return 5; }
     int operator "" _C(const char*, unsigned long) { return 6; }
   }

using ud_ops::operator"" _C;
#endif

void foobar()
   {
     int a = 9_LL;

#if 0
     a = "xyz"_CCS;
     a = 9.0_LD;

     int (*pf)(long double) = &operator "" _LD;
     a = pf(9.0L);

     a = 'a'_C;
     a = ud_ops::operator "" _C('a');

     a = L'a'_C;
     a = operator "" _C(L'a');

     a = "abc"_C;
     a = operator "" _C("abc",4);
#endif
   }

