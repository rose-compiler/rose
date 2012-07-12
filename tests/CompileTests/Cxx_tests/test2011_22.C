
// This is similar to test2011_19.C and test2011_20.C but requires more qualification.
class GenericHashTable
   {
     public:
          class Iterator
             {
               public:
                    Iterator() {}
             };
          class Inner_GenericHashTable
             {
               public:
                    class Iterator
                       {
                         public:
                              Iterator() {}
                       };
             };
   };

class GenericSpecialization
   {
     public:
       // Name qualification is required for since Iterator is not visible (which is different than being hidden).
       // This requires 2 name qualifications.
         class Iterator : public GenericHashTable::Inner_GenericHashTable::Iterator
             {
               public:
                    Iterator() {}
             };
   };
