
// in global scope (top of scope)

void foo()
   {
   // before if
     if (1)
        {
          int a;
       // in true block
        }
       else
        {
       // in false block
          int a;
        }
  // after if
   }

// in global scope (before x)

int x;

// in global scope (after x)

typedef struct tagTypedef 
   {
  // top of typedef
     int x;
  // middle of typedef
     int y;
  // bottom of typedef
   } ABC1;

union foo_union
   {
  // top of union
     int x;
  // middle of union
     int y;
  // bottom of union
   } ABC2;

struct foo_struct
   {
  // top of struct
     int x;
  // middle of struct
     int y;
  // bottom of struct
   } ABC3;

class foo_class
   {
  // top of class
     int x;
  // middle of class
     int y;
  // bottom of class
   } ABC4;

class tagClass 
   {
  // top of class
     int x;
  // middle of class
     int y;
  // bottom of class
   };

// in global scope (bottom of scope)

int functionPrototype();

// comment at end of file!










