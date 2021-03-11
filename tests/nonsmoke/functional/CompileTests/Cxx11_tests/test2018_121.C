// ROSE-40 (Kull)

namespace namespace1 {
  class Class1 {
  public:
    virtual double func_and_local1();
  };

  double func_and_local1();
}

double namespace1::Class1::func_and_local1() 
   {
     double func_and_local1;

  // Original code:  func_and_local1 = namespace1::func_and_local1();
  // Generated code: func_and_local1 = func_and_local1();
     func_and_local1 = namespace1::func_and_local1();

     return func_and_local1;
   }

#if 0
It gets this error:
rose_ROSE-40.cpp: In member function 'virtual double namespace1::Class1::func_and_local1()':
rose_ROSE-40.cpp:15:35: error: 'func_and_local1' cannot be used as a function
 func_and_local1 = func_and_local1();
                                   ^
Because this line:

  func_and_local1 = namespace1::func_and_local1();

gets unparsed to:

func_and_local1 = func_and_local1();
#endif
 
