#if 0
void method1() {
    #include "Inner.h"
	v1_rename_me = 10;
	v2_rename_me = 11;
	v3_rename_me = 12;
	v4_rename_me = 13;
}

void method2() {
	#include "Inner.h"
	v1_rename_me = 15;
	v2_rename_me = 16;
	v3_rename_me = 17;
	v4_rename_me = 18;
}

int main(int argc, char* argv[]) {

}
#endif



// Example of code to be segregated into two codes controled by an #ifdef ... #endif CPP directive.

#if 0
int abc_var_abc;
int abc_var_xyz;
#endif

#if 0
class A 
   {
     private:
       // int x;
          int abc_var_y;
          int abc_var_z;
          int x;

#if 0
     public:
          int foo1();
          int foo2();
          int abc_function_foo3();
#endif
   };

#endif

#if 0
void foobar()
   {
     abc_var_abc = 42;
     abc_var_abc = 43;

     int x;
   }
#endif


int xxx;




