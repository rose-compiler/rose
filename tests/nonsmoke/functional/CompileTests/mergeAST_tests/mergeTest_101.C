// This example demonstrates where a function is used 
// before it is declared (and never defined).
class ctype
   {
     public:
          void toupper()
             { x; this->do_toupper(); }

          int x;

          virtual void do_toupper();
    };
