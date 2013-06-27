class Test
   {
     public:
          Test* operator -> ();
          int getFormat();
   };

void foobar()
   {
     Test ref ;
  // ref.operator->()->getFormat();
#if 1
     ref.operator->()->getFormat();
#endif
#if 1
     ref->getFormat();
#endif
   }

