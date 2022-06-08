 
/* OUTLINED FUNCTION PROTOTYPE */
extern "C" void OUT_1_test_02_cpp_0(void **__out_argv);int abxyz; 
 
 
void foo(int );

extern "C" void OUT_1_test_02_cpp_0(void **__out_argv)
{
  int abxyz =  *((int *)__out_argv[0]);
//  ::abxyz = 42;
    abxyz = 42;
   *((int *)__out_argv[0]) = abxyz;
}
