struct X { int a; };

void foo()
   {
     int i = 0;
     for (i = 0; i < ((int)(sizeof(struct X))); i++) 
        {
        }
   }
