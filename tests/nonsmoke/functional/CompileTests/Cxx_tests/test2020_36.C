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
  // Original code: int *pbv = &(static_cast<V *>(static_cast<B *>(pd))->v);
  // Generated code: int *pbv = &(static_cast<class V *>  ((static_cast < class B * >  (pd)))) -> B_::v;
     int *pbv = &(static_cast<V*>(static_cast<B*>(pd))->v);
   }
