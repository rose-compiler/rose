struct B
   {
     int bi;
   };

struct X
   {
     int bi;
   };

struct C : private B
   {
     B *f() { return this; }
     int ci;
   };

struct Y : private X
   {
     X *f() { return this; }
     int ci;
   };

class D : private C, private Y
   {
     public:
          ::B *fb() { return C::f(); }
          int di;
   };


void foobar()
   {
     D d;
     B *bp = d.fb();
     &d;
     (D *)bp;
     bp->bi;
     ((D *)bp)->di;
     B &br = *d.fb();
     &(D &)br;
     ((D &)br).di;
     int D::*pmd = (int D::*)&B::bi;
     d.*pmd;
   }
