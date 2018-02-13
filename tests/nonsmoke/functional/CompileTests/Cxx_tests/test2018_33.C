class X
   {
     public:
          int i;
          X(int j) : i(j) { }
   };

int operator * (X  a);

// DQ (2/1/2018): Added case fo non-member operator&
X* operator & (X  a);

// DQ (2/1/2018): Added case fo non-member operator~
bool operator ~ (X  a);

// DQ (2/1/2018): Added case fo non-member operator!
bool operator! (X  a);

// DQ (2/1/2018): Added case fo non-member operator++
bool operator++ (X  a);

// DQ (2/1/2018): Added case fo non-member operator--
bool operator-- (X  a);

// I don't think there is a unary or operator.
// DQ (2/1/2018): Added case fo non-member operator|
// bool operator | (X  a);

void foobar()
   {
     X a (1);
     const X * pc = &a;

     int i3 = **pc;
   }
