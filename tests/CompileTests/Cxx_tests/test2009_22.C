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
#if 0  
   if (this != &rhs)
   {
         i = rhs.i;
   }
#endif   
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
//   j = old.j;
// EDG output is:  this->IdxSet::operator= ( old );
   IdxSet::operator= ( old );
}


