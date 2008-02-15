/* Unparser bug:

     struct MY_STRUCT { int x; } xStruct;
unparses to:
     struct MY_STRUCT xStruct;

     struct { int x; } xStruct;
unparses to:
     struct __T8687204 dans_typedef_of_a_struct;

Note: KCC translates 
         struct A { int x; } a;
      into
         struct A;  struct A { int x; };  struct A a;

Use of typedef does not even get to the unparser phase.
*/

#if 0
// typedef
struct
 structName
   {
     int x;
#if 1
   } xStruct;

// struct structName { int x; } yStruct;
// typedef
// structName yStruct;
#else
   };

typedef
structName xStruct;
#endif
#endif

#if 0
// Different ways of specifing enums (similar problems as presented for structs (above))
enum { a, b, c };
enum capitalLetters { A, B, C };
enum { aa, bb, cc } doubleLowerCaseLetters;
enum capitalDoubleLetters { AA, BB, CC } doubleLetters;

capitalLetters moreLetters;
capitalDoubleLetters moreDoubleLetters;
#endif

#if 0
class A
   {
     public:
          int x;
// This #if controls if the {...} is unparsed or not (the existance of a tag?)
#if 1
   };
#else
   } B;
#endif
#endif

#if 1
int
main ()
   {
     if (1)
        {
          struct structName { int x; } localXStruct;
          localXStruct.x = 42;
          return localXStruct.x;
        }

  // struct xStruct X;
  // struct structName X;
  // xStruct.x = 7;
  // A X;
  // X.x = 42;
  // return X.x;

     return 0;
   }
#endif


