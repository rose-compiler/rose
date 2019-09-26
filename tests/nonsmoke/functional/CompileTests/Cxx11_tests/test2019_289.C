class B
   {
     public:
          int i;
          int operator , (int j) { return ( i + j + 1 );}
          B(int j) : i(j) { }
   };

enum e_ { E0a_ = 0, E99a_ = 99 };
int operator , (B  s, e_ j) { return ( s.i + (int)j + 10 );}

class B2a
   {
     public:
          int i;
          int operator & () { return ( i + 1 );}
          B2a(int j) : i(j) { }
   };

void foobar()
   {
     B b (1);

  // Original code: int i1 = (b , 1);
  // Unparsed as:   int i1 = b , 1;
  // The code generation just needs to output "()",  Is this a precedence issue?
     int i1 = (b , 1);

     e_ e = E0a_;
     B2a b2 (1);
     int i2 = &b2;
   }

