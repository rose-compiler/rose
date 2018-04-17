// t0381.cc
// some games with elaborated type specifiers

// icc fails to reject: 1

struct C {};
struct D {};

int C;

enum E { e1 };
enum F { f1 };

int foo()
{
  {
    // would make use of 'D' below refer to typedef
    //ERROR(1): typedef struct C D;

    struct D d;
  }

  {
    // similar for enums
    //ERROR(2): typedef enum E F;
    
    enum F f;
  }


  struct C c;

  return C;
}


