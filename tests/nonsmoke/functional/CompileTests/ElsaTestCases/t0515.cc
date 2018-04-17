// t0515.cc
// assertion-failure-inducing input from acovea_4.0.0-4_acovea.cpp
// nested DQTs in return type

// This testcase has another interesting aspect: EDG and GCC both
// seem to regard the templatized 'sqrt' as not matching any
// call site if their attempt to compute the return type encounters
// errors due to member lookup failure, for example because the
// _M_type members aren't declared.  I find this behavior unexpected,
// and do not know of any justification for it in the standard, but
// it might actually be correct.

// Actually, re-reading the relevant sections, I see:
//   - 14.8.3p1: function templates lead to candidates iff template
//               argument deduction succeeds
//   - 14.8.2p1: template argument deduction can fail when 
//               substitution of arguments for parameters causes
//               an invalid type to be generated, such as referencing
//               a non-existent member

template < typename, bool > 
struct __enable_if {
  typedef int _M_type;
};

template < typename _Tp >
struct __is_integer {
  static bool const _M_type = false;
};

long double sqrt (long double __x);

template < typename _Tp >
typename __enable_if < double, __is_integer < _Tp >::_M_type >::_M_type
sqrt (_Tp __x);

inline double acosh (const double &x)
{
  return sqrt (x);
}

int foo(int x)
{
  return sqrt(x);
}

int bar(int *x)
{
  // this one has to use the template
  return sqrt(x);
}

double zoo(double x)
{
  // force use of template despite reasonably good match with
  // the non-template
  return sqrt<>(x);
}
