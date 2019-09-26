// ROSE-1910: (C++03) Template argument not qualified when typedef'd in template class

//  This bug comes from minimizing a failure when unparsing template from the AST in kripke .
//  For the issue to happen, we need:
//   - typedef in A
//   - var init in A's ctor
//   - defined ctor in B

namespace N0000 
   {
     using IIII = int;

     template <typename T>
     struct AAAA
        {
          typedef T v_t;
          v_t v;
          AAAA() : v(0) {}
        };
   }

// namespace N1111 
//  {
#if 0
     void foobar()
        {
          N0::A<N0::I>();
        }
#else
     struct BBBB : public N0000::AAAA<N0000::IIII> 
        {
          BBBB() {}
        };
#endif
// }


