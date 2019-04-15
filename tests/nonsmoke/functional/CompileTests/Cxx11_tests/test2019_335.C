class A { };

class B { };

class C { };

void foobar()
   {
  // Can we build a pointer to member for which the base type is a pointer to member?
  // I think we can only make this more complex by building a pointer to a pointer to member.
     A B::**prm;
   }
