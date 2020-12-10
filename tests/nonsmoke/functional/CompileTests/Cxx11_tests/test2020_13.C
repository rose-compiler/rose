class RawHashTable
   {
     public:
          typedef int Iterator;
   };

class GenericHashTable : public RawHashTable
   {
     public:
          GenericHashTable ( );
          GenericHashTable ( int sz );

     public:
          typedef int Iterator;
       // class Iterator { };
   };


class SurfaceFacetHashTable : public GenericHashTable
   {
     public:
          class Iterator // : public GenericHashTable::Iterator
             {
             };

          Iterator a;
          GenericHashTable::Iterator b;
          RawHashTable::Iterator c;
   };


