
class Internal_Index
   {
     public:

          friend Internal_Index operator+ ( const Internal_Index & Lhs , int i );
          // { return Internal_Index ( Lhs.Base + i, Lhs.Count, Lhs.Stride, Lhs.Index_Mode ); }
          friend Internal_Index operator+ ( int i , const Internal_Index & Rhs );
          // { return Internal_Index ( Rhs.Base + i, Rhs.Count, Rhs.Stride, Rhs.Index_Mode ); }
#if 0
          friend Internal_Index operator- ( const Internal_Index & Lhs , int i );
          // { return Internal_Index ( Lhs.Base - i, Lhs.Count, Lhs.Stride, Lhs.Index_Mode ); }
          friend Internal_Index operator- ( int i , const Internal_Index & Rhs );
          // { return Internal_Index ( i - Rhs.Base, Rhs.Count, -Rhs.Stride, Rhs.Index_Mode ); }

       // Unary minus allows the stride to be reversed!
          Internal_Index operator- () const;
#endif
   };




   
