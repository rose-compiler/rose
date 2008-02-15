// Test code for mid-level rewrite

#if 0
#if 0
int y;
#endif

int a;

#if 0
int z;
#endif
#endif

#if 0
void foo (int i);

int xyz;

void foo (int i) {};
#endif

// int abcdefg;

class X
   {
     public:
//        int a;
          X ();
          void foobar (int i){};
   };

#if 1
X::X(){};

int main()
   {
     int x,y;
     switch(x)
        {
          case 0: 
               y++;
               break;
          default: 
               y++;
               break;
        }
   }
#endif

