struct B { int bi; };

class D : public B
   {
     public:
          void foo()
             {
               int D::*dp = &D::bi;
               int B::*bp = &B::bi;
             };
   };

