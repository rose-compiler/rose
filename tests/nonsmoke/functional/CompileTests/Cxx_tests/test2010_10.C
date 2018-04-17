// Hi Dan,
// The bug in the code below is showing up in a little over 30 important files:
//

class GenericBase
{
  public:
     GenericBase() {}
} ;

class GenericHashTable : public GenericBase
{
  public:
         class Iterator;
  friend class Iterator;
  class Iterator : public GenericBase
  {
    public:
       Iterator() {}
  } ;
} ;

class GenericSpecialization : public GenericHashTable
{
  public:
         class Iterator;
  friend class Iterator;
#if 1
  class Iterator : public GenericHashTable::Iterator
  {
    public:
       Iterator() {}
  } ;
#endif
} ;
