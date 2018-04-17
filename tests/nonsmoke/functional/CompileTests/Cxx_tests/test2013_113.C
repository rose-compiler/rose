
template<class type> class  QList
   {
     public:
          type *at( int i );
   };

template<class T> class LockingPtr
   {
     public:
          T* operator-> () const;
   };

class GroupList;

class ClassDef
   {
     public:
#if 1
          int getOutputFileBase() const;
#else
          void getOutputFileBase() const;
#endif
          LockingPtr<GroupList> partOfGroups() const;
   };

class GroupList : public QList<ClassDef> {};

#if 1
int ClassDef::getOutputFileBase() const 
   {
     return partOfGroups()->at(0)->getOutputFileBase();
   }
#else
void ClassDef::getOutputFileBase() const 
   {
#if 1
     partOfGroups()->at(0)->getOutputFileBase();
#else
     partOfGroups()->at(0);
#endif
   }
#endif

