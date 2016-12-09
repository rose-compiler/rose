class X
   {
     public:
          X (int n)
             {
             }
         
#if 1
          friend X operator- ( const X & Lhs , int i );
#endif
#if 1
          inline friend X operator+ ( const X & Lhs , int i )
         { return X(i); }
#endif
   };


   
