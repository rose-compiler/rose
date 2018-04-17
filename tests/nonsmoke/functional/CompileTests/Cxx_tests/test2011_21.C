
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

class GenericSpecialization : public GenericHashTable
   {
     public:
#if 1
       // Name qualification is required for since Iterator is not visible (which is different than being hidden).
       // This requires 1 name qualification.
         class MyIterator : public Inner_GenericHashTable::Iterator
             {
               public:
                    MyIterator() {}
             };
#endif
   };
