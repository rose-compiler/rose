// The bug is present for classes (not just templates).
class QListIterator
   {
     public:
          int *operator++();
   };

void NEW_write()
   {
     QListIterator dnli1;
     for ( ; ; ++dnli1,++dnli1)
        {
        }
   }

