#if 1
template<class type> class QListIterator
   {
     public:
          type *operator++();
   };

class DotNode {};
#else

// The bug is present for classes (not just templates).
class QListIterator
   {
     public:
          int *operator++();
   };
#endif

void NEW_write()
   {
  // QList<DotNode> *nl = 0L;
  // QListIterator<DotNode>  dnli1(*nl);
     QListIterator<DotNode>  dnli1;
  // QListIterator dnli1;
  // for ( ; ; ++dnli1,++dnli1)
     for ( ; ; ++dnli1,++dnli1)
        {
        }
   }

