int foobar()
   {
     for(int i=0; i < 10; i++)
        {
          class XYZ {};

          class Y
             {
               public:
                 // This friend declaration causes an error in ROSE.
                    friend class XYZ;
             };
        }
   }
