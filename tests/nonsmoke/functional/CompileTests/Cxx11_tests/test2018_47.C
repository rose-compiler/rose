namespace xxx
   {
     struct struct1
        {
          int x;
        };
   }

struct struct2
   { 
     xxx::struct1 array1[2]; 
   };

struct2 local2 = {xxx::struct1 {1} , xxx::struct1 {2}};
