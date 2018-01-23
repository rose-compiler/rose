int main()
   {
     int a;
     int b;
     int x;
     switch(x)
        {
#if 0
          case 1: {}
               int y;
               a++;
#endif
#if 1
          case 2: {}
               int z;
               b++;
#endif
#if 0
          default: 
             {
               int zz;
               x++;
             }
#endif
        }
   }  





#if 0
   default:
     {x--;break;}
#endif
