
#if 0
struct A
   {
     struct B { A *a; B *b; };
   };
#endif


#if 1
// global typedef that will be hidden
typedef int my_integer_t;

// global typedef that will not be hidden
typedef int your_integer_t;

namespace X 
   {
  // global name qualification required (because a matching named typedef is built below)
     typedef ::my_integer_t X_integer_t;

  // global name qualification required (because the result matches the name of the base type)
     typedef ::my_integer_t my_integer_t;

  // global name qualification not required
     typedef your_integer_t X_your_integer_t;
   }

void foo()
   {
  // This typedef hides the global typedef "my_integer_t"
     typedef int my_integer_t;

  // global name qualification required
     typedef ::my_integer_t foo_integer_t;
   }

#if 1
struct A { struct B{}; };
namespace X
   {
     struct A { struct C{}; };
     typedef ::A::B x1_type;
     typedef A::C   x2_type;

     void foo()
        {
          typedef ::A::B x1_type;
          typedef A::C   x2_type;
        }
   }

void foobar()
   {
     struct A { struct D{}; };
     typedef ::A::B x3_type;
     typedef A::D   x4_type;
   }
#endif
#endif

