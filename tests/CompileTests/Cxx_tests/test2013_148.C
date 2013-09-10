template <class TElem> class ValueVectorOf
   {
     public :
          ValueVectorOf();

          TElem* fElemList;
   };

class X
   {
     public:
          class Y
             {
             };
   };

void foo( const X::Y y 
#if 0
, const X::Y *y_ptr
#endif
)
   {
#if 1
  // This is unparsed as: extern class ValueVectorOf< X::const Y  > snapshot();
  // The error is: "X::const Y" instead of "const X::Y".
     ValueVectorOf< const X::Y > snapshot();
#endif
#if 0
     ValueVectorOf< const X::Y > *snapshot_ptr = new ValueVectorOf< const X::Y >();
#endif
#if 1
     ValueVectorOf< const X::Y*  > *snapshot_ptr_2 = 0L;
#endif
   }
