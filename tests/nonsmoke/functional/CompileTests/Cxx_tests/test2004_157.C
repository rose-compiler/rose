
// This code demonstrates an error in the unparsing 
// of operator[] (and likely other operators as well)

class A
   {
     public:
          A();
          A( int *x, int y);
          int & operator[](int i);
          A *operator->() const { return Aptr; }
          A& operator*() const  { return *Aptr; }

          A*  Aptr;
          A** Aptrptr;
   };

template <typename T>
class B
   {
     public:
          B();
          B( int *x, int y);
          int & operator[](int i);
          B<T>* Bptr;
          B<T>** Bptrptr;
   };

void foobar ( int *x, int y );

void foo()
   {
      A a;
      A* aptr = &a;
      A** aptrptr = &aptr;

      aptr->operator[](1);
      (*aptr)[1];
      (*aptrptr)->operator[](1);
      (*(*aptrptr))[1];

      (aptr->Aptr)->operator[](1);
      (*(aptr->Aptrptr))->operator[](1);


      foobar( &((*(aptr->Aptrptr))->operator[](1) ), 42 );
      A a2( &((*(aptr->Aptrptr))->operator[](1) ), 42 );

      B<int> b;
      B<int>* bptr = &b;
      B<int>** bptrptr = &bptr;

      bptr->operator[](1);
      (*bptr)[1];
      (*bptrptr)->operator[](1);
      (*(*bptrptr))[1];

      (bptr->Bptr)->operator[](1);
      (*(bptr->Bptrptr))->operator[](1);

      foobar( &((*(bptr->Bptrptr))->operator[](1) ), 42 );
      B<int> b2( &((*(bptr->Bptrptr))->operator[](1) ), 42 );

#if 0
  // Unparses to be:
  //  class A expr (&(*self -> object)->[*a0_0.bp()],((short )0));
  //        A expr (&(((*(self->object))->operator[]((*(a0_0.bp()))))),0);
#endif
   }

