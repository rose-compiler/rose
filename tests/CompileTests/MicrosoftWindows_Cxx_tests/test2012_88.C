int i = 1;
int j = 2;

class X 
   {
     public:
          X () {}
          X (int i) {}
   };

X *x3 = new X[2];
X *x4 = new X;
X *x5 = new X[i+j];

