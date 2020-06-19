#include <assert.h>

#include <DepInfo.h>
#include <sstream>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

class DepEDDTypeInfo : public DepInfoImpl
{
  DepType t;
 public:
  DepEDDTypeInfo( DepType _t, int dim1, int dim2, bool p, int cl)  // p: precise or not,  cl: common level
    : DepInfoImpl( dim1, dim2,p, cl) { t = _t; }
  DepEDDTypeInfo( const DepEDDTypeInfo &that)
    : DepInfoImpl( that ) { t = that.t; }
  virtual ~DepEDDTypeInfo() {}
  virtual DepInfoImpl* Clone() const { return new DepEDDTypeInfo(*this); }

  virtual DepType GetDepType() const { return t; }
};

class DepEDDRefInfo : public DepEDDTypeInfo
{
protected:
  AstNodePtr src, snk;
public:
  virtual AstNodePtr SrcRef() const { return src; }
  virtual AstNodePtr SnkRef() const { return snk; }
  DepEDDRefInfo( DepType _t, int dim1, int dim2,
                 AstNodePtr _src, const AstNodePtr& _snk, bool p, int cl) // dim1, dim2 are rows and cols for the matrix, p cl is common level
    : DepEDDTypeInfo(_t, dim1, dim2,p,cl), src(_src),  snk(_snk) {}
  DepEDDRefInfo( const DepEDDRefInfo &that) : DepEDDTypeInfo(that),
       src(that.SrcRef()), snk(that.SnkRef()) {}
  virtual ~DepEDDRefInfo() {}
  virtual DepInfoImpl* Clone() const { return new DepEDDRefInfo(*this); }
};


DepInfo DepInfoGenerator:: GetTopDepInfo()
{
  return DepInfo();
}

DepInfo DepInfoGenerator:: GetBottomDepInfo(int nr, int nc, int commLevel  )
{
  assert(commLevel <= nr && commLevel <= nc);
  DepInfoImpl *impl = new DepInfoImpl( nr, nc, false, commLevel);
  impl->edd.Initialize(DepRel(DEPDIR_ALL));
  for (int i = 0; i < commLevel; ++i) {
     impl->Entry(i,i) = DEPDIR_LE;
  }
  return impl;
}

DepInfo DepInfoGenerator::GetIDDepInfo(int nr, bool p)
{
  DepInfoImpl *impl = new DepInfoImpl( nr, nr, p, nr);
  impl->edd.Initialize(DepRel(DEPDIR_ALL));
  for (int i = 0; i < nr; i++)
     impl->Entry(i,i) = DepRel(DEPDIR_EQ,0);
  return impl;
}

DepInfo DepInfoGenerator:: GetDepInfo( int nr, int nc, bool p, int commLevel)
{
  assert(commLevel <= nr && commLevel <= nc);
  return new DepInfoImpl(nr, nc, p, commLevel);
}

DepInfo DepInfoGenerator:: GetDepInfo( int nr, int nc, DepType t, bool p, int commLevel )
  { return (t==DEPTYPE_NONE)? DepInfo(new DepInfoImpl( nr, nc, p, commLevel))
                     : DepInfo(new DepEDDTypeInfo( t, nr, nc, p, commLevel )); }

DepInfo DepInfoGenerator::
GetDepInfo( int nr, int nc, DepType t, const AstNodePtr& srcRef, const AstNodePtr& snkRef,
            bool p, int commLevel)
  { return ( (t & DEPTYPE_DATA) || (t & DEPTYPE_INPUT) )?
                      DepInfo(new DepEDDRefInfo( t, nr, nc, srcRef, snkRef,p, commLevel))
                     :  DepInfo(new DepEDDTypeInfo(t, nr, nc, p, commLevel)) ; }

std::string DepType2String(DepType t)
{
  switch (t) {
  case DEPTYPE_CTRL: return "CTRL_DEP;";
  case DEPTYPE_TRUE: return  "TRUE_DEP;";
  case DEPTYPE_OUTPUT: return  "OUTPUT_DEP;";
  case DEPTYPE_ANTI: return  "ANTI_DEP;";
  case DEPTYPE_INPUT: return  "INPUT_DEP;";
  case DEPTYPE_SCALAR: return  "SCALAR_DEP;";
  case DEPTYPE_BACKSCALAR: return  "SCALAR_BACK_DEP;";
  case DEPTYPE_IO: return  "IO_DEP;";
  case DEPTYPE_DATA: return  "DATA_DEP;";
  case DEPTYPE_BACKCTRL: return  "BACKCTRL_DEP;";
  case DEPTYPE_TRANS: return  "TRANS_DEP;";
  case DEPTYPE_NONE : return  "TYPE_NONE";
  default: assert(false);
  }
}

std::string DepInfo :: toString() const
{
  int num1, num2;
  CarryLevels(num1,num2);
  std::stringstream out;
  out << " Distance Matrix size:"<< rows() << "*" << cols()<<" ";
  out << DepType2String(GetDepType()) << " commonlevel = " << CommonLevel() << " ";
  out << "CarryLevel = ("<<num1 << "," << num2 << ") \n";
  if (is_precise())
      out << " Is precise ";
  else
      out<<  " Not precise ";
  out << AstInterface::AstToString(SrcRef())<<AstInterface::getAstLocation(SrcRef())<<"->" << AstInterface::AstToString(SnkRef())<<AstInterface::getAstLocation(SnkRef())<<"\n";
  for (int i = 0; i < rows(); i++) {
    for (int j = 0; j < cols(); j++) {
       out << Entry( i, j).toString() << ";";
    }
    out << "||";
  }
  out << "::";
  return out.str();
}

bool DepInfo:: operator |= (const DepInfo &d2)
{
  return UpdateMatrix(*this, d2, DepRel::UnionUpdate);
}

DepInfo operator | ( const DepInfo &info1, const DepInfo &info2)
{
  DepInfo result(info1);
  result |= info2;
  return result;
}

bool DepInfo :: operator &= ( const DepInfo &d2)
{
  return UpdateMatrix(*this, d2, DepRel::IntersectUpdate);
}

bool DepInfo:: operator *= ( const DepInfo &info2)
{
  if (cols() == 0) return false;
  if (cols() != info2.rows()) {
    std::cerr << "info1=" << toString() << "; info2=" << info2.toString() << "\n"; assert(false);
  }
  DepInfo info1(*this);
  int commLevel1 = info1.CommonLevel();
  int commLevel2 = info2.CommonLevel();
  if (commLevel1 > commLevel2) commLevel1 = commLevel2;
  *this = DepInfoGenerator::GetDepInfo(info1.rows(), info2.cols(), DEPTYPE_TRANS, commLevel1);
  for (int i = 0; i < info1.rows(); i++) {
    for (int j = 0; j < info2.cols(); j++) {
      for (int k = 0; k < info1.cols(); k++) {
        DepRel r = info1.Entry(i,k) * info2.Entry(k,j);
        Entry(i,j) &= r;
      }
    }
  }
  return info1 != (*this);
}

DepInfo operator & ( const DepInfo &info1, const DepInfo &info2)
{
  DepInfo result(info1);
  result &= info2;
  return result;
}

bool DepInfo :: ClosureEntries()
{
  bool mod = false;
  assert(rows() == cols());
  for (int i = 0; i < rows(); i++) {
    for (int j = 0; j < cols(); j++) {
      DepRel e1 = Entry(i,j);
      if (e1.Closure()) {
        Entry(i,j) = e1;
        mod = true;
      }
    }
  }
  return mod;
}

DepType ReverseDepType( DepType t)
{
  if (t == DEPTYPE_TRUE)
     return DEPTYPE_ANTI;
  else if (t == DEPTYPE_ANTI)
     return DEPTYPE_TRUE;
  else
     return t;
}

DepInfo Reverse(const DepInfo &info)
{
  if (info.IsID())
    return info;
  DepType t = ReverseDepType( info.GetDepType());
  DepInfo result=
        DepInfoGenerator::GetDepInfo(info.cols(), info.rows(), t,
                                     info.SnkRef(), info.SrcRef(), info.is_precise(),
                                     info.CommonLevel());
  for (int i = 0; i < info.rows(); i++) {
    for (int j = 0; j < info.cols(); j++) {
      result.Entry(j,i) = Reverse(info.Entry(i,j));
    }
  }
  return result;
}

void DepInfo ::  TransformDep (DepInfo &dep, DepDirection dir) const
{
  assert( rows() == cols());
  DepInfo info( dep);
  if (dir & DEP_SRC) {
     dep = (*this) * info  ;
  }
  if (dir & DEP_SINK) {
     dep = info * (*this);
  }
}

DepInfo Closure( const DepInfo &info )
{
  DepInfo result(info);
  result.ClosureEntries();
  return result;
}

bool DepInfo :: operator == ( const DepInfo &d2) const
{
  if (CountRefHandle<DepInfoImpl>:: operator ==(d2))
     return true;
  if (rows() != d2.rows() || cols() != d2.cols() || CommonLevel() != d2.CommonLevel())
     return false;
  for (int i = 0; i < rows(); i++) {
    for (int j = 0; j < cols(); j++) {
      if (  Entry(i,j) != d2.Entry(i,j) )
        return false;
    }
  }
  return true;
}

bool DepInfo :: operator < ( const DepInfo &d2) const
{
  if (rows() != d2.rows() || cols() != d2.cols() || CommonLevel() != d2.CommonLevel())
     return false;
  for (int i = 0; i < rows(); i++) {
    for (int j = 0; j < cols(); j++) {
      if ( !(Entry(i,j) < d2.Entry(i,j)) )
        return false;
    }
  }
  return true;
}

bool DepInfo :: operator <= ( const DepInfo &d2) const
{
  if (rows() != d2.rows() || cols() != d2.cols() || CommonLevel() != d2.CommonLevel())
     return false;
  for (int i = 0; i < rows(); i++) {
    for (int j = 0; j < cols(); j++) {
      if ( !(Entry(i,j) <= d2.Entry(i,j)) )
        return false;
    }
  }
  return true;
}

DepInfo operator * (const DepInfo &info1, const DepInfo &info2)
{
  DepInfo result=info1;
  result *= info2;
  return result;
}

void DepInfo::InsertLoop( int level, DepDirection dir)
{
  const DepInfo tmp(*this);
  int d1 = rows(), d2 = cols();
  int incr1 = (dir & DEP_SRC)? 1 : 0;
  int incr2 = (dir & DEP_SINK)? 1 : 0;

  assert( (incr1 == 0 || level < d1) && (incr2 == 0 || level < d2) );
  UpdateRef().Reset(d1+incr1,d2+incr2);
  if (d1 == 0 || d2 == 0)
    return;

  int i,j;
  if ( level < d1) {
    for ( i = 0; i < level; ++i)  {
      if ( level < d2) {
         for ( j = 0; j < level; ++j)
           Entry(i,j) = tmp.Entry(i,j);
      }
      else
         j = 0;
      for (; j < d2; ++j)
         Entry(i,j+incr2) = tmp.Entry(i,j);
    }
  }
  else
    i = 0;
  for (; i < d1; ++i) {
     if (level < d2) {
        for ( j = 0; j < level; ++j)
           Entry(i+incr1,j) = tmp.Entry(i,j);
     }
     else
        j = 0;
     for ( ; j < d2; ++j)
         Entry(i+incr1,j+incr2) = tmp.Entry(i,j);
  }
  if (incr1) {
     for ( j = 0; j < d2+incr2; ++j)
         Entry(level, j) = DEPDIR_ALL;
  }
  if (incr2) {
     for (i = 0; i < d1+incr1; ++i)
         Entry(i,level) = DEPDIR_ALL;
  }
  if (level <= CommonLevel() && incr1 && incr2)
     ++CommonLevel();
  assert( CommonLevel() <= rows() && CommonLevel() <= cols());

}

void DepInfo:: DistLoop( int level)
{
/* QY: 3/4/2018: changing common level here is problematic for some tests
  if ( level < CommonLevel());
      CommonLevel() = level;
*/
  assert( CommonLevel() <= rows() && CommonLevel() <= cols());
}

void DepInfo:: RemoveLoop( int level, DepDirection dir)
{
  const DepInfo tmp(*this);
  int d1 = rows(), d2 = cols();
  if (level < CommonLevel())
     --CommonLevel();

  int incr1 = (dir & DEP_SRC)? 1 : 0;
  int incr2 = (dir & DEP_SINK)? 1 : 0;

  assert(d1 >= incr1 && d2 >= incr2);
  UpdateRef().Reset(d1-incr1,d2-incr2);

  int m1 = (level < d1)? level : d1;
  int m2 = (level < d2)? level : d2;
  int i,j;
  for ( i = 0; i < m1; ++i)  {
    for ( j = 0; j < m2; ++j)
         Entry(i,j) = tmp.Entry(i,j);
    for (j = level+incr2; j < d2; ++j)
         Entry(i,j-incr2) = tmp.Entry(i,j);
  }
  for ( i = level + incr1; i < d1; ++i) {
     for ( j = 0; j < m2; ++j)
         Entry(i-incr1,j) = tmp.Entry(i,j);
     for ( j = level+incr2 ; j < d2; ++j)
         Entry(i-incr1,j-incr2) = tmp.Entry(i,j);
  }
  assert( CommonLevel() <= rows() && CommonLevel() <= cols());
}

void DepInfo::MergeLoop( int index1, int index2, DepDirection dir)
{
  if (dir & DEP_SRC) {
    for (int j = 0; j < cols(); j++) {
      Entry(index1,j) = Entry(index2, j) ;
    }
  }
  if (dir & DEP_SINK) {
    for (int j = 0; j < rows(); j++) {
      Entry(j,index1) = Entry(j,index2);
    }
  }
}

void DepInfo::SwapLoop( int index1, int index2, DepDirection dir)
{
  if (dir & DEP_SRC) {
    for (int j = 0; j < cols(); j++) {
      DepRel r = Entry(index1, j);
      Entry( index1, j) = Entry(index2, j);
      Entry( index2, j) = r;
    }
  }
  if (dir & DEP_SINK) {
    for (int j = 0; j < rows(); j++) {
      DepRel r =  Entry(j, index1);
      Entry( j, index1) = Entry(j, index2);
      Entry( j, index2) = r;
    }
  }
}

void DepInfo::AlignLoop( int index, int align, DepDirection dir)
{
  if (dir & DEP_SRC) {
     for (int j = 0; j < cols(); j++) {
       Entry(index, j).IncreaseAlign(align);
     }
  }
  if (dir & DEP_SINK) {
    for (int j = 0; j < rows(); j++) {
       Entry(j,index).IncreaseAlign(-align);
    }
  }
}

int DepInfo:: CarryLevel() const
{
  int minlevel, maxlevel;
  CarryLevels(minlevel, maxlevel);
  return minlevel;
}

void DepInfo :: CarryLevels( int &minLevel, int &maxLevel) const
{
  minLevel = -1;
  maxLevel = -1;
  // int nr = rows(), nc = cols();
  int num = CommonLevel();
  for (int i = 0; i < num; i++) {
    DepRel r = Entry(i,i);
    DepDirType t = r.GetDirType();
    int align1 = Entry(i,i).GetMinAlign();
    int align2 = Entry(i,i).GetMaxAlign();
    bool carry = true, notcarry = true;

    switch (t) {
    case DEPDIR_EQ:
         if (align1 == 0 && align2 == 0)
             carry = false;
         else if (align1 > 0 || align2 < 0)
             notcarry = false;
         break;
    case DEPDIR_LE:
         if ( align2 < 0)
             notcarry = false;
         break;
    case DEPDIR_GE:
         if (align1 > 0)
            notcarry = carry = false;
    default: break;
    }
    if (carry) {
      if (minLevel < 0)
         minLevel = maxLevel = i;
       else maxLevel = i;
    }
    if ( !notcarry )  {
        maxLevel = i;
        break;
    }
    else maxLevel = i;
  }
  if (minLevel < 0) {
     if (GetDepType() == DEPTYPE_BACKSCALAR)
        minLevel = maxLevel = num-1;
     else
        minLevel = maxLevel = num;
  }
}

bool DepInfo :: IsTop( ) const
{
  if (ConstPtr() == 0)
     return true;
  for (int i = 0; i < rows(); ++i) {
    for (int j = 0; j < cols(); ++j) {
      if (Entry(i,j).IsTop())
         return true;
    }
  }
  return false;
}

bool DepInfo :: IsBottom( int level ) const
{
  if (level >= rows() || level >= cols())
     return true;
  for (int i = level; i < rows(); i++) {
    for (int j = level; j < cols(); j++) {
      if ( ! Entry(i,j).IsBottom())
         return false;
    }
  }
  return true;
}

bool DepInfo :: IsID() const
{
  if (rows() != cols())
     return false;
  for (int i = 0; i < rows(); i++) {
    for (int j = 0; j < cols(); j++) {
      if ( (i== j && Entry(i,i) != DEPDIR_EQ) ||
           (i!=j && !Entry(i,j).IsBottom()) )
       return false;
    }
  }
  return true;
}

bool IsValidDep(  const DepInfo &d, int num)
{
  if (d.GetDepType() == DEPTYPE_IO)
      return true;

  for (int i = 0; i < num; i++) {
    DepRel r = d.Entry(i,i);
    DepDirType t = r.GetDirType();
    int align = d.Entry(i,i).GetMaxAlign();
    switch (t) {
    case DEPDIR_NONE:
         return true;
    case DEPDIR_LE:
    case DEPDIR_EQ:
       if ( align < 0)
         return true;
       break;
    case DEPDIR_GE:
    case DEPDIR_ALL:
         return false;
    default: break;
    }
  }
  return true;
}

