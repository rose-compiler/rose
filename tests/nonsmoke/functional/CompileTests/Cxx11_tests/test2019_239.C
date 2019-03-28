
typedef unsigned long size_t;

class rational
   {
     public:
          int x, y;
          rational(int i, int j);
          rational(const rational &r);
          rational operator +(int i) const;
          operator int() const;
   };

class omni
   {
     public:
          int i;
          signed char c;
          long L;
          double d;
          static omni **dp;
          long la[4];
          long double lda[4];
          short a[4];
          short &r1;
          short *p2;
          rational xy;
          enum palette { RED, WHITE, BLUE };
          palette color;
          int omni::*pm;
          static int omni::*spm;
          signed char & rfi();
          signed char & rfo();
          operator int() const;
          operator rational();
          omni();
          omni(int ii);
          omni(const rational &r);
         ~omni();
     private:
          void construct(int ii);
   };


class over
   {
     public:
          int i;
          signed char c;
          long L;
          double d;
          over *po;
          static over *spo;
          static int new_calls;
          static int array_new_calls;
          static int delete_calls;
          static int array_delete_calls;
          static size_t requested_size;

          over * operator -> ();
          over * operator &  ();
          void * operator new(size_t n);
          void   operator delete(void *p);
          void * operator new[](size_t n);
          void   operator delete[](void *p);

          over & operator =  (const over &b);
          operator double();

          over();
          over(int ii);
   };


void foobar()
   {
     omni m = 91;
     over v = 52;
     int omni::*pim = &omni::i;
     double over::*pdv = &over::d;
     long over::*plv = reinterpret_cast<long over::*>(pim);
  // chk(reinterpret_cast<int omni::*>(plv) == pim);
  // ieq(m.*reinterpret_cast<const int omni::*>(pim), 91);
     rational omni::*prm = reinterpret_cast<rational omni::*>(pdv);
  // chk(reinterpret_cast<double over::*>(prm) == pdv);
     reinterpret_cast<double over::*>(prm) == pdv;
  // deq(v.*reinterpret_cast<double over::*>(prm), 52);
     typedef signed char &(omni::*pfm_t)();
     typedef over *(over::*pfv_t)();
     pfv_t pfv = reinterpret_cast<pfv_t>(&omni::rfo);
  // chk(reinterpret_cast<pfm_t>(pfv) == &omni::rfo);
     (m.*reinterpret_cast<pfm_t>(pfv))() = '1';
  // ieq(m.c, '1');
   }

