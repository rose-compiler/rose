namespace GenericHashTable
   {
     class Iterator
        {
          public:
               Iterator() {}
        };
   }

class GenericSpecialization
   {
     public:
       // Name qualification is required for since Iterator is not visible (which is different than being hidden).
       // This requires 1 level name qualification.
         class Iterator : public GenericHashTable::Iterator
             {
               public:
                    Iterator() {}
             };
   };
