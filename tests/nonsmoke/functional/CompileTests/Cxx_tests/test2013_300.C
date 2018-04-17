// This is called: Template non-type parameter overload
// This si discussed in: http://stackoverflow.com/questions/17313649/how-can-i-distinguish-overloads-of-templates-with-non-type-parameters

// DQ (8/6/2013): I think that niether of these template functions
// can be called because they are ambigous at compile-time.

// This is the template function for member functions.
template <unsigned int T> void test() {}

// This is the template function for member data.
template <signed long T> void test() {}

void foo()
   {
     const unsigned int x = 1;
     const signed long y  = -1;

#if 0
  // DQ (8/12/2013): These template functions can be defined but not called (because they can't be disambiguated based on different integer kinds).
     test<x>();
     test<y>();
#endif
   }

#if 0
// The suggested solution is to add the type to the template parameter list
template<typename T, T param>
struct template_const_impl;

template <int module>
struct template_const_impl<int, module>
{
    static void apply(int &a, int &b)
    {
        a = a & module;
        b = b % module;
    }
};

template<bool x>
struct template_const_impl<bool, x>
{
    static void apply(int &a, int &b)
    {
        const int w = x ? 123 : 512;
        a = a & w;
        b = b % w;
    }
};

template <typename T, T param>
void template_const(int &a, int &b)
   {
     return template_const_impl<T, param>::apply(a, b);
   }

int main()
   {
     int i = 512, j = 256;
     template_const<int, 123>(i, j);
     template_const<bool, true>(i, j);
   }

// The main function can be implemented using this C++11 feature:
#define TEMPLATE_CONST(x) template_const<decltype(x), x>

int main()
   {
     int i = 512, j = 256;
     TEMPLATE_CONST(123)(i, j);
     TEMPLATE_CONST(true)(i, j);
   }
#endif

