/*
Hi Dan,

While attempting to use my translator on ROSE I encountered a couple of
bugs regarding the unparsing of operator-overloaded code.  Below is a
test case:

class A
   {

     public:
          void operator=(int a);
          void operator++();

   };

class B : public A
   {

     public:
          void operator++();

   };

void A::operator=(int a)
   {
   }

void B::operator++()
   {
     A::operator=(42);
     A::operator++();
   }

void A::operator++()
   {
   }

int main()
   {
     B b;
     ++b;
   }

When run through the identitiyTranslator I get this:


class A 
{
  public: void operator=(int a);
  void operator++();
}

;

class B : public A
{
  public: void operator++();
}

;

void A::operator=(int a)
{
}


void B::operator++()
{
  =42;
  ++(*(this) );
}


void A::operator++()
{
}


int main()
{
  class B b;
  ++b;
}

The first line in B::operator++ is just nonsense, and the second line
is calling the wrong class (it calls B's rather than A's operator++).
The first bug prevents many ROSE source files in src/ROSETTA/src from
being unparsed correctly.

Thanks,
-- Peter 

*/


class A
   {

     public:
          void operator=(int a);
          void operator++();

   };

class B : public A
   {

     public:
          void operator++();

   };

void A::operator=(int a)
   {
   }

void B::operator++()
   {
     A::operator=(42);
     A::operator++();
   }

void A::operator++()
   {
   }

int main()
   {
     B b;
     ++b;
   }

