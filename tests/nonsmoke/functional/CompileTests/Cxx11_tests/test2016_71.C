
// template<typename _Tp, _Tp __v> struct integral_constant { };

struct __do_is_implicitly_default_constructible_impl
   {
     template <typename _Tp> static void __helper(const _Tp&);

  // template <typename _Tp> static void __test(decltype(__helper<const _Tp&>({}))* = 0);
  // template <typename _Tp> void foobar(decltype(__helper<const _Tp&>({}))* = 0);
  // template <typename _Tp> void foobar(__helper<const _Tp&>({})* = 0);
  // template <typename _Tp> void foobar(decltype(__helper<const _Tp&>({}))*);
  // template <typename _Tp> void foobar( decltype(__helper<const _Tp&>({})) );

  // Moved error to return type.
     template <typename _Tp> decltype(__helper<const _Tp&>({})) foo();

     //     template <typename _Tp> decltype(__helper<const _Tp&>({{}})) foobar();
  };


__do_is_implicitly_default_constructible_impl XXX;

void foobar()
{
  XXX.foo<int>();
}
