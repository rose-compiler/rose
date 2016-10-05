// #include <stdio.h>

#if 1
// Main program
int main()
   {
     const int n = 10;
     int x = 42;
     for (int i=0; i < n; i++)
        {
          x++;
        }
     return x;
   }
#endif


#if 0
class X
   {
     public:
          int x;
   };

// Main program
int main()
{
   const int n = 10;
   int A[n];
   X *pointer;
   for (int i=0; i < n; i++)
      {
        A[i];
        pointer->x = 0;
      }
   
   return 0;
}
#endif

#if 0
typedef int TypeOfX;     
typedef char* string;

void aw(string s);
void ar(string s);
void bw(string s);
void br(string s);

// int operator+ (int i, int j); // error

class A {};
A operator, (A i, A j);
A operator, (A i, int j);
// A operator, (int i, A j);
int operator, (int i, A j);
// int operator, (void* i, int j); // error

// postfix
// TypeOfX operator++(TypeOfX & x,int n);
// prefix
// TypeOfX operator++(TypeOfX & x);

// TypeOfX operator*(TypeOfX* x);

void foo()
   {
     int x,y,z;
#if 0
   // x;  ->  b(),x,a();
      x;
      br("x"),x,ar("x"),x;
#endif
#if 1
   // x = 0;  ->  (b(),x) = 0, a(), x;
      x = 0;
      (bw("x"),x) = 0;
      bw("x"),x = 0;
#endif
#if 0
     x = y;
     (((bw("x"),x) = ((br("y"),y),ar("y"), y)), aw("x"), x);
#endif
#if 0
  // x = y + z;  -> ((b(),x) = ((b(),y) + ((b(),z), az(), z), ay(), y), ax(), x);
     x = y + z;
     (((bw("x"),x) = ((br("y"),y) + ((br("z"),z), ar("z"), z), ar("y"), y)), aw("x"), x);

     (((bw("x"),x) = (((br("y"),y) + (ar("y"),br("z"),z)), ar("z"))), aw("x"), x);

     ((bw("x"),x) = (((br("y"),y) + (ar("y"),br("z"),z)), ar("z"))), aw("x");
#endif
#if 0
     local = x = y + z;
     ((bw("x"),x) = (((br("y"),y) + (ar("y"),br("z"),z)), ar("z"))), aw("x");
#endif

TypeOfX fooPassByValue( TypeOfX x);
TypeOfX fooPassByReference( TypeOfX & x);

#if 0
     x;
     br("x"),x;
     x = 0;
     bw("x"),x = 0;
     x = foo();
     bw("x"),x = foo();
     x = fooPassByValue(x);
     bw("x"),x = fooPassByValue((br("x"),x));
     x = fooPassByReference(x);
     bw("x"),x = fooPassByReference(x);
     x = y;
     bw("x"),x = br("y"),y;
     x = y + z
     bw("x"),x = br("y"),y + br("z"),z;
     x++;
     TypeOfX brw(TypeOfX & xprime,string s);
     brw(x,"x")++;
     x = x + 1;
     ++x;

  // for x = 3
     z = x++ + x; // 7
     z = x + x++; // 6 
     z = ++x + x; // 8
     z = x + ++x; // 7

  // postfix case
     z = x++ + x;
     z = ((x = x + 1),x-1) + x;
     bw("z"),z = ((bw("x"),x = br("x"),x + 1),x-1) + (br("x"),x);

  // postfix operator
     TypeOfX operator++(TypeOfX & x,int n)
        {
          return bw("x"),x = (br("x"),x)+1,x-1;
        }

  // prefix case
     z = ++x + x;
     z = (x = x + 1) + x;
     (bw("z"),z) = ((bw("x"),x) = (br("x"),x)+1) + (br("x"),x);

  // prefix operator
     TypeOfX operator++(TypeOfX & x)
        { 
          return br("x"),x,x+1;         
        }

     TypeOfX* bd ( TypeOfX* p );
     void* bd_void ( void* p );

  // pointers
     TypeOfX *p;
     local = *p;
     local = *(bd( (br("p"),p) ));

  // How to convert a point to a class to another class (in case this helps define a counter example)
     class B {};
     class A { operator B (A* a); };
     foo ( B b );
     A* a;
     foo (a);
#endif

#if 0
  // arrays
     int & b_array_w(string s, int & value); // C++ specific
     int * b_array_w(string s, int* value); // C or C++ specific
     int array[10];
     array[0] = 0;
     (b_array_w("array element",((br("array"),array)[10])) = 0); // for C++
     (*b_array_w("array element",&((br("array"),array)[10])) = 0); // for C or C++
#endif
   }


// member data access
class C 
   {
     public:
          int x;
          int *p;
          void foo();
   };

C c;
C *c_pointer;
void foobar()
   {
     c.x = 0;
     (bw("c.x"),c.x) = 0;
     c.foo();
     (br("c"),c).foo();

     c.p;
     c_pointer->p;
     (br("c_pointer"),c_pointer)->foo();
   }
#endif      
