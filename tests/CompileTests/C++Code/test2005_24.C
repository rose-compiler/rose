int i = 1;
int j = 2;

// Test of nonconstant aggregate constant
int a[3] = { 1, i+j, 42 };

int b[3];

class X 
   {
     public:
          X () {}
          X (int i) {}
   };

X x1[3] = { 1, i+j, 42 };
X x2[3];

// Problem code
// std::vector<Ray<MT> > *R = new vector<Ray<MT> >[pL->nSend];

X *x3 = new X[2];
X *x4 = new X;
X *x5 = new X[i+j];

// This prevents an error from: assert (curr_source_sequence_entry != NULL);
// within the code above!
int main()
   {
     return 0;
   }

