class array
   {
     public:
          friend array & max ( const array & X , const array & Y );
          friend array & max ( const array & X , float y );
#if 0
          friend array & operator+ ( const array & X , float y );
          friend array & operator+ ( const array & X , double y );
          array & operator+ ( const array & X );
          friend array & operator- ( const array & X , double y );
          array & operator- ( const array & X );
#endif
   };

