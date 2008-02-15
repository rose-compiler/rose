
class A
   {
     public:
       // void foo();
          A operator=(A a);
       // A operator+(A a);
   };

class B : public A
   {
     public:
       // void foo();
          A operator=(A a);
       // A operator+(A a);
   };

void foobar()
   {
  // A a;
     B b;

  // Calling base class operator=  "b.A::operator=(b);", but unparses to "b=b;"
  // In the hidden declaration list for class "B" "B::operator=(A a)" is in the hidden list
  // instead of "A::operator=(A a)"  Is this the way it is supposed to be?
     b.A::operator=(b);
   }

