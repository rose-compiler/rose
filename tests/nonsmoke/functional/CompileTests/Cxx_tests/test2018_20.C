int foobar()
   {
     class X;

     class Y
        {
          public:
            // This friend declaration causes an error in ROSE.
               friend class X;
        };

  // class X {};

  // X xxx;
   }
