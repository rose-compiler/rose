// This example demonstrates where a function is used 
// before it is declared (and never defined).
class ctype
   {
     public:
          void toupper() const
             { this->do_toupper(); }

          virtual void do_toupper() const;
    };

