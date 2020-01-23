
class RawHashTable
   {
     public:
          class Iterator
             {
             };
   };

class GenericHashTable : public RawHashTable
   {
     public:
          GenericHashTable ( );
          GenericHashTable ( int sz );

     public:
          class Iterator
             {
             };
   };


class SurfaceFacetHashTable : public GenericHashTable
   {
     public:
          class Iterator : public GenericHashTable::Iterator
             {
             };
   };


