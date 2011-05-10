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
       // This requires 0 name qualification.
         class MyIterator : public GenericHashTable::Iterator
             {
               public:
                    MyIterator() {}
             };
   };
