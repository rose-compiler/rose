
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
   };

#if 0
template <typename T>
class B
   {
     public:
          B();
          B( int *x, int y);
          int & operator[](int i);
          B<T>* Bptr;
          B<T>** Bptrptr;
          B *operator->() const { return Bptr; }
          B& operator*() const  { return *Bptr; }
   };
#endif

void fooError()
   {
      A a;
      A *a_pointer;

   // The following two statements are exactly the same (but expressed with different syntax)
      a->operator[](1);

   // This is the same as the statement above but using explicit operator function syntax
      a.operator->()->operator[](1);

   // Here, "a[1];" is really the same as "a.operator[](1)", but we want 
   // to unparse it as "a[1]" to keep the generated code simple!
      a[1];

   // This is correct code, but causes an error in the unparser!
      (*(a->Aptr))[1];

   // This is a meaningless statement but is legal C++ (C++ trivia)
   // Note that it is the same as "a->;" but that is not legal code! (this now works in ROSE!)
      a.operator->();
      a->Aptr;
      a_pointer->operator->();
      a_pointer->operator->()->Aptr;

   // Note that "(*a_pointer)[1];" and "(*a)[1];" are not the same (since one is the derefencing 
   // of a pointer and the other is the derefence operator, "operator*()", called on the object!).
      (*a_pointer)[1];
      (*a)[1];

   // Also, these are the same (if A defines such operators as operator*() and operaotr[]()).
      (*a)[1];
      a.operator*().operator[](1);
   }

