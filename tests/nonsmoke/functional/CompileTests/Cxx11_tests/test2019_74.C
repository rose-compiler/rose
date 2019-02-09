struct B { int bi; };

class D : private B
   {
     public:
          int D::*bar(int B::*p) const { return p; }
   };

