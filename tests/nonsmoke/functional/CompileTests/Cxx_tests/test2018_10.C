void 
bar(int& w)
   {
     ++w;
   }

int 
main(int, char**)
   {
     int z = 3;
     int a = 5 + z + 9;
     int b = (6 - z) * (a + 2) - 3;
     bar(b);
     while (b - 7 > 0)
        {
          b-=5; 
          --b;
        }

     while (true) {
          --b; LLL: if (b <= -999) return 0;
          if (!(b > 2)) break;
        }

     for (b = 0; b < 10; ++b) {
          ++z;
     }

     for (int z2 = 7 + z * 5; z2 + 9 < b % 10; ++*(&z2 + 5 - 5))
        {
          (a += 7) += 7; 
          ++(++a);
        }
     b = -999;
     goto LLL;
   }
