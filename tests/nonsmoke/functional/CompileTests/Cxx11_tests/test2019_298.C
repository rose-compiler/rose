class c_
   {
          int k;

     public:
          int get_k() const volatile;  // decl of member fn
          c_() : k(833) { }
   };

int c_::get_k()const volatile { return k; } // def of member fn

int (c_::*pf)()const volatile=&c_::get_k; // ptr to member fn

const volatile c_ cvo_; // a const volatile obj


void foobar()
   {
     cvo_.get_k();
     (cvo_.*pf)();
   }
