template <int I> 
class A 
   {
     static char s[I+1];
	};

template <int I> char A<I>::s[I+1] = { 0 };

void foo()
   {
     A<2> aa;
   }
