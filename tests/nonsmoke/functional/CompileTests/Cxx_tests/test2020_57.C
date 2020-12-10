namespace N
   {
     class X
        {
       // The bug in later versions of GNU (later than 7.4, at least) is the N:: name qualification.
       // likely the "public:" should not be output either. I think this is a result of a recent bug 
       // fix in ROSE (last week of August 2020).
       // Original code:          friend int     Xvalue(const       X &x);
       // Generated code: public: friend int N::Xvalue(const class X &x);
          friend int Xfunction(const X &x);
        };

     int Xfunction(const X &x);
   }

