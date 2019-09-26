template<class T, class... T2>
int A(T2..., T) 
   { 
     return 1; 
   }

int B(int a, float& b) 
   {
     const int c = a;

  // Unparsed as: return ::A< int  > (a,b,c);
     return A<int, int, int>(a, b, c);
   }

void foobar()
   {
     float b = 2.f;
     int a = B(1, b);

     const int c = a;
     A<int, int, int>(a, b, c);
   }
