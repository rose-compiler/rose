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
       // This requires 1 name qualification.
          class Iterator : public GenericHashTable::Iterator
             {
               public:
                    Iterator() {}
             };
   };
