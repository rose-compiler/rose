//#include "optionDeclaration.h"

class OptionDeclaration
{
	public:
		// Constructors designed to trigger auto-promotion of constructor arguments
		OptionDeclaration( char* s, int value );
		OptionDeclaration( char* s, double value );
		OptionDeclaration( char* s, char* value );
};

OptionDeclaration option1 ("VERSION",1.0);
OptionDeclaration option2 ("PACKAGE","BABEL_Preprocessor_4");


class A
   {
     public:
          int x;

       // access functions
          void set_x(long A_i);
		  void set_x(int A_i);

		  void foo(int a);
		  void foo(int a, int b);

		  void bar(int a, int b);
		  void bar(float a, float b);

		  void ptest(int a);
		  void ptest(int *a);
    //      int get_x();

	//	  class B
	//	  {
	//		  int a;
	//	  };
   };

