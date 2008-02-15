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
OptionDeclaration option2 ("PACKAGE","BABEL_Preprocessor");

// Not sure where to put this function within BABLE
void foo ( int myVariable );

class A
   {
     public:
          int x;

       // access functions
          void set_x(long A_i);
          int get_x();
   };

class B
   {
     public:
          int y;

       // access functions
          void set_y(long B_j);
          int get_y();
   };

#if 0
// Nothing in in the automated generation of Babel requires a main function to be defined
int
main ()
   {
     A a;
   }
#endif
