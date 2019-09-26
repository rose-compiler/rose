class A 
   {
     int y, z;

     public: 
          A() 
             {
               y = 0; 
               z = 0; 
             } 
  
       // Constructor delegation  
          A(int z) : A() 
             {
               this->z = z; // Only update z 
             }
   };

