class V
   {
     public:
          int v;
   };

class NV
   {
     public:
          int nv;
   };

class B : public NV, public virtual V { };

class C : public NV, public virtual V { };

class D : public B, public C { };


void foobar()
   {
     D d;
     D *pd = &d;
  // Original code: int *pbv = &(static_cast<V_ *>(static_cast<B_ *>(pd))->v);
     int *pbv = &(static_cast<V*>(static_cast<B*>(pd))->v);
  // Original code: int *pcv = &(static_cast<V_ *>(static_cast<C_ *>(pd))->v);
     int *pcv = &(static_cast<V*>(static_cast<C*>(pd))->v);
   }
