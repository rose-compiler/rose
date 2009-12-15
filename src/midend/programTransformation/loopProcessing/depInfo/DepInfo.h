

#ifndef DEP_INFO_H
#define DEP_INFO_H

#include <CountRefHandle.h>
#include <IteratorTmpl.h>
#include <AstInterface.h>

#include <DepRel.h>
#include <Matrix.h>
#include <iostream>

typedef enum { 
  DEPTYPE_NONE = 0,       // B00000000
  DEPTYPE_TRUE = 1,       // B00000001
  DEPTYPE_ANTI = 2,       // B00000010   
  DEPTYPE_OUTPUT = 4,     // B00000100
  DEPTYPE_SCALAR = 8,     // B00001000
  DEPTYPE_BACKSCALAR = 16,// B00010000
  DEPTYPE_INPUT = 32,     // B00100000
  DEPTYPE_ARRAY = 39,     // B00100111  //union of true, anti, output, input dependencies
  DEPTYPE_IO = 64,        // B01000000
  DEPTYPE_DATA = 95,      // B01011111 //union of array, scalar/backscalar,IO,w/o input
  DEPTYPE_CTRL = 128,     // B10000000
  DEPTYPE_BACKCTRL = 256, //B1 00000000
  DEPTYPE_ALL = 479,      //B1 11011111 //All but input dependence
  DEPTYPE_TRANS = 512    //B10 00000000
} DepType;
std::string DepType2String( DepType t);
typedef enum {DEP_SRC = 1, DEP_SINK = 2, DEP_SRC_SINK = 3} DepDirection;

typedef Matrix<DepRel> DepEDD;

class DepInfoImpl 
{
   DepEDD edd;
   bool precise;
   int commonlevel;
 protected:
   virtual ~DepInfoImpl() {}
   DepInfoImpl( int dim1, int dim2, bool p, int cl) 
        : edd( dim1, dim2, (dim1 == 0 || dim2 == 0)? 0 : 1) ,
          precise(p), commonlevel(cl) {}

   DepInfoImpl(const DepInfoImpl &that) : edd(that.edd), precise(that.precise),
                     commonlevel(that.commonlevel) {}
 public:
   int rows() const { return edd.rows(); }
   int cols() const { return edd.cols(); }
   DepRel& Entry(int index1, int index2) const 
       { return edd.Entry(index1,index2); }
   DepEDD& GetEDD() { return edd; }
   const DepEDD& GetEDD() const { return edd; }

   int CommonLevel() const {  return commonlevel; }
   int& CommonLevel()  {  return commonlevel; }
   bool is_precise() const { return precise; }
   void set_precise() { precise = true; }

   virtual DepInfoImpl* Clone() const 
     { return new DepInfoImpl(*this); }
   virtual DepType GetDepType() const { return DEPTYPE_NONE; }
   // Liao, 11/4/2008. Additional information for SCALAR_DEP and SCALAR_BACK_DEP
   // Reuse DEPTYPE_TRUE, DEPTYPE_ANTI, DEPTYPE_OUTPUT etc for them
   virtual DepType GetScalarDepType() const { return DEPTYPE_NONE; }
   virtual void SetScalarDepType(DepType st) {}
   virtual AstNodePtr SrcRef() const { return AST_NULL; }
   virtual AstNodePtr SnkRef() const { return AST_NULL; }

   void Reset(int srcdim, int snkdim) { edd.Reset(srcdim, snkdim); }
   void Initialize(  const DepRel &init) { edd.Initialize(init); }
 friend class CountRefHandle<DepInfoImpl>;
 friend class DepInfoGenerator;
};

inline DepInfoImpl* Clone(const DepInfoImpl& that) { return that.Clone(); }
//!Extended Dependence Model information
// DepRel (distance direction)-> EdpEDD (matrix of distance direction)-> DepInfoImpl (extra info)
// -->DepEDDTypeInfo (dependence type)--> DepInfo (with reference count) 
class DepInfo : protected CountRefHandle <DepInfoImpl>
{
  DepInfo( DepInfoImpl *impl) : CountRefHandle<DepInfoImpl>(impl) {}
 public:
  typedef DepRel Elem;

  DepInfo() {}
  DepInfo(int) {}
  DepInfo( const DepInfo &that) : CountRefHandle<DepInfoImpl>(that) {}
  DepInfo & operator = (const DepInfo &that)
    { CountRefHandle <DepInfoImpl>::operator =(that); return *this; }
  const DepInfo* operator->() const { return this; }
  DepInfo* operator->() { return this; }


  int rows() const { return (ConstPtr() == 0)? 0 : ConstRef().rows(); }
  int cols() const { return (ConstPtr() == 0)? 0 : ConstRef().cols(); }
  DepType GetDepType() const  
    { return (ConstPtr()==0)? DEPTYPE_NONE: ConstRef().GetDepType(); }
    
  DepType GetScalarDepType() const  
    { return (ConstPtr()==0)? DEPTYPE_NONE: ConstRef().GetScalarDepType(); }
  void SetScalarDepType(DepType st)
    { 
      if (ConstPtr()==0) 
        return;
      else 
        NonConstRef().SetScalarDepType(st); 
    }
  AstNodePtr SrcRef() const { return (ConstPtr()==0)?AST_NULL:ConstRef().SrcRef(); }
  AstNodePtr SnkRef() const { return (ConstPtr()==0)?AST_NULL:ConstRef().SnkRef(); }
  int CommonLevel() const { return  (ConstPtr()==0)?-1:ConstRef().CommonLevel(); }

  int& CommonLevel() { return  UpdateRef().CommonLevel(); }

  const DepRel& Entry(int index1, int index2) const 
    { return ConstRef().Entry(index1,index2); }
  DepRel& Entry(int index1, int index2)
    { return UpdateRef().Entry(index1, index2); }
  DepEDD& GetEDD() { return UpdateRef().GetEDD(); }
  const DepEDD& GetEDD() const { return ConstRef().GetEDD(); }
  void Initialize( const DepRel &init) { UpdateRef().Initialize(init); }

  void Dump() const { std::cerr << toString() << std::endl; }
  std::string toString() const;
  void CarryLevels( int &minLevel, int &maxLevel) const;
  int CarryLevel() const;
  bool IsTop( ) const;
  bool IsBottom( int level = 0 ) const;
  bool IsID() const;
  bool is_precise() const { return (ConstPtr() == 0)? true : ConstRef().is_precise(); }
  void set_precise() { UpdateRef().set_precise(); }

  bool operator &= ( const DepInfo &d2);
  bool operator |= ( const DepInfo &d2);
  bool operator *= (const DepInfo& d2);
  bool ClosureEntries();

  bool operator <= ( const DepInfo &d2) const ;
  bool operator < ( const DepInfo &d2) const ;
  bool operator == (const DepInfo &d) const;
  bool operator != (const DepInfo &d) const { return ! operator ==(d); }
  void InsertLoop( int level, DepDirection dir);
  void RemoveLoop( int level, DepDirection dir);
  void DistLoop( int level);
  void MergeLoop( int index1, int index2, DepDirection dir);
  void SwapLoop( int index1, int index2, DepDirection dir);
  void AlignLoop( int index1, int index2, DepDirection dir);

  void TransformDep( DepInfo &dep, DepDirection dir) const;
 friend class DepInfoGenerator;
};

inline bool IsNIL(const DepInfo &info) { return info.IsTop(); }
bool IsValidDep( const DepInfo &info, int commLevel);
typedef IteratorImpl<DepInfo> DepInfoConstIteratorImpl;
typedef IteratorImpl<DepInfo&> DepInfoUpdateIteratorImpl;
typedef IteratorWrap<DepInfo,DepInfoConstIteratorImpl> DepInfoConstIterator;
typedef IteratorWrap<DepInfo&, DepInfoUpdateIteratorImpl> DepInfoUpdateIterator;

DepInfo operator * (const DepInfo& d1, const DepInfo& d2);
DepInfo operator & ( const DepInfo &d1, const DepInfo &d2) ;
DepInfo operator | ( const DepInfo &d1, const DepInfo &d2);
DepInfo Reverse( const DepInfo &d);
DepInfo Closure( const DepInfo &d);

//! A builder for DepInfo
class DepInfoGenerator
{
 public:
  static DepInfo GetTopDepInfo();
  static DepInfo GetBottomDepInfo( int nr, int nc, int commLevel = 0);
  static DepInfo GetIDDepInfo( int nr, bool precise );
  static DepInfo GetDepInfo(int nr, int nc, bool precise, int commLevel);
  static DepInfo GetDepInfo(int nr, int nc, DepType t, bool precise, int commLevel);
  static DepInfo GetDepInfo(int nr, int nc, DepType t, 
                            const AstNodePtr& srcRef, const AstNodePtr& snkRef, 
                            bool precise, int commLevel);
};

#endif
