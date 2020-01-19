
void foobar()
   {
     struct X { typedef int T; };
     X::T *p = new X::T;
     p;
     p->X::T::~T();
     delete p;
   }


