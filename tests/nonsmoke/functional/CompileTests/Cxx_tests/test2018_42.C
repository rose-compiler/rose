class X
   {
     public:
          int i;
          X(int j) : i(j) { }

       // DQ (2/1/2018): Added case of member operator--(prefix operator)
          X & operator-- ();

       // DQ (2/1/2018): Added case of member operator-- (postfix operator)
          X operator-- (int i) const;
   };

// DQ (2/1/2018): Added case of non-member operator++ (prefix operator)
X & operator++ (X  a);

// DQ (2/1/2018): Added case of non-member operator++ (postfix operator)
X operator++ (X  a, int i);

#if 0
// DQ (2/1/2018): Added case of non-member operator--(prefix operator)
X operator-- (X  a);

// DQ (2/1/2018): Added case of non-member operator-- (postfix operator)
X operator-- (X  a, int i);
#endif

// I don't think there is a unary "or" operator.
// DQ (2/1/2018): Added case of non-member operator|
// bool operator | (X  a);

void foobar()
   {
     X a (1);

     X b = 1;
  // b = --(a--);
     b = a--;

  // b = ++((*pc)++);
     b = a++;


   }
