template <int I> 
class A 
   {
     static char s[I+1];

  // This demonstrates a problem in the EDG/ROSE connection. It is related
  // to a EDG 4.8 specific problem that I notice in test2013_242.C (in 
  // convert_constant_template_parameter(): case tpck_expression).

  // unsigned long const src_len = 7;
     const unsigned long src_len = 7;
  // char buf[src_len + 1];
  // char buf[src_len];
     char buf[src_len + 1];
	};

// template <int I> const unsigned long A<I>::src_len = 7;

template <int I> char A<I>::s[I+1] = { 0 };

void foo()
   {
     A<2> aa;
   }
