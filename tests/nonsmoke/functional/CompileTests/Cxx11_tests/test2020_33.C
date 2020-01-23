
class B
   {
     public:
          int i;
          int operator , (int j) { return ( i + j + 1 );}
          B(int j) : i(j) { }
   };

enum e { E0a = 0, E99a = 99 };

int operator , (B  s, e j) { return ( s.i + (int)j + 10 );}

void foobar()
   {
     B b (1);

     int i1 = (b , 1);
   }

