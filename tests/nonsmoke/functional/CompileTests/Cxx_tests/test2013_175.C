namespace B
   {
     enum E2 { en21, en22 };
   }

int main()
   {
     using B::E2; // causes error :
     return 0;
   } 
