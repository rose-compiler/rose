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
// X::X(){};

int global_x;

int main()
   {
     int local_x;

  // int x,y;
     int x;
     int y;
     switch(x)
        {
          case 0: 
            // Note that processing of "y" requires construction of declaration of "y" in the prefix, 
            // but we must handle the case where the declaration of "y" is a part of a previous 
            // transformation, so file equality is an insufficent test for inclusion of declarations 
            // within the prefix.
               y++;
               break;
          default: 
               y++;
               break;
        }
   }
#endif

