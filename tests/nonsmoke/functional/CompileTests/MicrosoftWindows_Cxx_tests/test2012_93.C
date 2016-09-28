class A
   {
     public:
          int & operator[](int i);
          const int & operator[](int i) const;
   };

#if 1
class X
   {
     public:
          void set( A & data ) const
             {
               int nz = 1;
               for (int z = 0; z < nz; z++)
                  {
                    data[z] = 42;
                  }
             }
   };
#endif

#if 1
void
set()
   {
     A data;
  // int x = data[42];
     data[0] = 42;
   }
#endif

