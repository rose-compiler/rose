#define AS_ARGUMENT int y
#define AS_ARG_ON_NEXT_LINE int y
#define A_SYMBOL this
#define WITHOUT_ARG int x;
#define WITH_ARGS(ARG_1) ARG_1;
#define WITH_TWO_EXPRESSIONS if(x==y) x=2; x++;
#define SYNTACTIC  }else x++;
#define SYNTACTIC2 x)
#define A_STATEMENT return 0
#define A_TYPE int

void foo(AS_ARGUMENT)
   {};

void bar(int x,
         AS_ARG_ON_NEXT_LINE)
   {};


class X{

     int x;
     void x_foo(){

          A_SYMBOL->x;  
     }
};


int main(){



     
WITHOUT_ARG
WITH_ARGS(int y)
WITH_TWO_EXPRESSIONS
A_TYPE h;
if(false){
SYNTACTIC

//if(SYNTACTIC2{};
	
	
   A_STATEMENT;
};
