class X
   {
     bool operator== ( const X & x ) const;

     bool operator!= ( const X & x ) const
        {
       // return !(operator==(x));
          return !(this->operator==(x));
        }
   };
