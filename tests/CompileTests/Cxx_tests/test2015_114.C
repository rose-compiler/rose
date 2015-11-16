
struct X { int a; };

class Y { int a,b; };

void foo()
   {
     for (int i = 0; i < ((int)(sizeof(class Y)/sizeof(struct X))); i++) 
        {
        }
   }
