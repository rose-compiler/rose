
namespace XXX {
typedef const int const_int;
}

class X
   {
     public:
          X (XXX::const_int x);
   };

X x = 42;
