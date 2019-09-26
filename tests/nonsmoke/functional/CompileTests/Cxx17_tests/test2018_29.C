// [[fallthrough]] attribute

void f(int n) 
   {
     void g(), h(), i();
    switch (n)
       {
         case 1:
         case 2:
              g();
              [[fallthrough]];
         case 3: // warning on fallthrough discouraged
              h();
         case 4: // implementation may warn on fallthrough
              i();
              [[fallthrough]];// ill­formed
       }
  }


