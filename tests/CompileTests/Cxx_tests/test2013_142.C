template <class TElem> class ValueVectorOf
   {
     public :
          ValueVectorOf( int x );
         ~ValueVectorOf();

          TElem* fElemList;
   };

template <class TElem> ValueVectorOf<TElem>::~ValueVectorOf()
   {
  // This is a bug in ROSE.
     fElemList[7].~TElem();

  // This works fine.
     fElemList->~TElem();
   }

void foo()
   {
     ValueVectorOf< int > snapshot(42);
  // ValueVectorOf< int > snapshot();
   }
