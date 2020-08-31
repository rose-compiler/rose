namespace N
   {
     class X
        {
          friend int Xfunction(const X &x);
          public:
               X(int ii) : i(ii) { }
               class Y
                  {
                    friend const char * Yfunction(const Y &y);
                    public:
                         Y(const char *pp) : p(pp) { }
                    private:
                         const char *p;
                  };
          private:
               int i;
        };
     int Xfunction(const X &x) { return x.i; }
     const char *Yfunction(const X::Y &y) { return y.p; }
   }

void foobar()
   {
     N::X x(3);
     const char *p = "abcdefg";
     N::X::Y y(p);
     N::Yfunction(y)[N::Xfunction(x)];
   }
