class GenericHashTable
   {
     public:
          class Iterator
             {
               public:
                    Iterator() {}
             };
   };

class GenericSpecialization : public GenericHashTable
   {
     public:
       // Name qualification is required for since Iterator is not visible (which is different than being hidden).
       // This requires 0 name qualification.
          class MyIterator : public Iterator
             {
               public:
                    MyIterator() {}
             };
   };
