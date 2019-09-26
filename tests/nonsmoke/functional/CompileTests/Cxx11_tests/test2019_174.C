
namespace N
{
class base_12
   {
     public:
          class nested_class
             {
               public:
                    nested_class();
             };
     base_12();
   };
}

// struct X : public base_12::nested_class
struct X : public N::base_12, public N::base_12::nested_class
   {
  // The call to nested_class() is ambiguous (even though it is the same ctor).
     X(); // : base_12::nested_class() {}
   };


X::X() : N::base_12::nested_class() {}
