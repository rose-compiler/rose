 class Class1 {
  void func3();
  void func2();
};

template < typename t_t_parm1, typename t_t_parm2 >
inline void t_func1( t_t_parm2&& parm1 ) {}

struct struct1 {};

void Class1::func3() 
   {
     t_func1< struct1 >( [=] 
        {
          this->func2();
        } );
   }

#if 0
The issue is that
           this->func2();
gets unparsed to:
        ->  func2 ();
resulting in the error:
rose_ROSE-39.cxx(22): error: expected an expression
        ->  func2 ();
        ^
#endif

