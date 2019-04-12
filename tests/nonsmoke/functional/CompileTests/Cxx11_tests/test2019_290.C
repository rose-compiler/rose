class B
   {
     public:
          int operator , (int j);
   };

enum e_ { E0a_ = 0, E99a_ = 99 };
int operator , (B  s, e_ j);

void foobar()
   {
     B b;

  // Original code: int i1 = (b , 1);
  // Unparsed as:   int i1 = b , 1;
  // The code generation just needs to output "()",  Is this a precedence issue?
     int i1 = (b , 1);
   }

