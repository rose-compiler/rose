// Liao, 5/21/2009
// A test case extracted from spec cpu2006's 450.soplex

class IdxSet
{
public:
  int i;
  IdxSet& operator= (const IdxSet& set);
//  IdxSet():i(0){}
  
};

IdxSet& IdxSet::operator=(const IdxSet& rhs)
{
   if (this != &rhs)
   {
         i = rhs.i;
   }
   return *this;
}

// derived class--------------
class DIdxSet: public IdxSet
{
  public:
    int j;
  //copy constructor
   explicit DIdxSet(const DIdxSet& old);
};

DIdxSet::DIdxSet(const DIdxSet& old)
   : IdxSet()
{
   j = old.j;
   IdxSet::operator= ( old );
}


