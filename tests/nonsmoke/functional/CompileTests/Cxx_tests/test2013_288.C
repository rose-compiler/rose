// This is the smallest example of the recent bug reported by Jeff.

class A 
   {
     public:
          class Iterator 
             {
               public:
                    Iterator(A* x);
             };
   };


class X : public A
   {
     public:
         class Iterator : public A::Iterator 
             {
               public:
                    Iterator(X* x) : A::Iterator(x) {};
             };
   };


#if 0
// This is a bug and unparsed incorrectly as: X::Iterator::Iterator(class X x) : Iterator(x) {}
X::Iterator::Iterator (X* x) 
   : A::Iterator(x)
   {
   }
#endif


