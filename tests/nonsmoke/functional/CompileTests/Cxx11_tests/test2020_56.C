namespace N
   {
     enum E { C };
   }

void foobar()
   {
  // This is a bug specific to the pseudo destructor.
     N::E e;

  // Original code: e.N::E::~E();
  // Unparses as:   e.~E();
     e.N::E::~E();

  // The good news is that this is simpler than a member function (since we can't take the address of a SgPseudoDestructorRefExp.
  // &(e.N::E::~E());
   }

