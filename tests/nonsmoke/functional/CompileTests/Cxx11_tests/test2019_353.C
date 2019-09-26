
class A 
   {
     int z;
     public: 
          A();
  
       // Constructor delegation  
           A(int z) : A()
             {
               this->z = z; // Only update z 
             }
   };

