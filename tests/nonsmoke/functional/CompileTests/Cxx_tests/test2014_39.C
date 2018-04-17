char *index ();

class Vector
   {
     public:
          void clear() {}
   };

class TreeIntVectSet
   {
     public:
          static Vector index;
   };

void clearStaticMemory()
   {
     TreeIntVectSet::index.clear();
   }

