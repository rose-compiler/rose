class A
   {
     public:
       // Even if there are multiple functions with the same name (overloaded) we don't want name qualification.
          A (int i);
          A (int i, int j);
   };

