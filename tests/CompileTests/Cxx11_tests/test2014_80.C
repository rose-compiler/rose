template<typename _Tp, typename ... _Args>
struct __is_nary_constructible
   {
  // This demonstrates the bug: In convert_expression(): case enk_sizeof_pack: not implemented: expression kind enk_sizeof_pack (not fixed).
     static_assert(sizeof ... (_Args) > 1,"Only useful for > 1 arguments");
   };

