#ifndef LOOP_TRANSFORMATION_INTERFACE_H
#define LOOP_TRANSFORMATION_INTERFACE_H

#include <list>
#include <string>
#include <iostream>
#include "AstInterface.h"
#include "SymbolicVal.h"
#include "AnalysisInterface.h"
#include <cstdio>

//! Base pure virtual interface to Array, Used by ArrayInterface, ArrayUseAccessFunction etc.
class ArrayAbstractionInterface {
 public:
  //! Check if 's' is an array access. 
  // Return the array node and list of subscripts if yes and requested
  virtual bool IsArrayAccess( AstInterface& fa,
                                 const AstNodePtr& s, AstNodePtr* array = 0,
                                 AstInterface::AstNodeList* index = 0) = 0 ;
  //! Get the lower and upper bound of an array's dim'th dimension
  // Return true if successful.                          
  virtual bool GetArrayBound( AstInterface& fa,
                                 const AstNodePtr& array, 
                                 int dim, int &lb, int &ub) = 0;
  //! Create an array access node to array 'arr' using a list of subscripts 'index'
  virtual AstNodePtr CreateArrayAccess( AstInterface& fa, const AstNodePtr& arr,
                                AstInterface::AstNodeList& index) = 0;
  virtual ~ArrayAbstractionInterface() {}
};

class ArrayUseAccessFunction 
  : public ArrayAbstractionInterface, public FunctionSideEffectInterface
{
  std::string funcname;
  ArrayAbstractionInterface* prev;
  FunctionSideEffectInterface* prev1;
 public:
  ArrayUseAccessFunction( const std::string& fn, ArrayAbstractionInterface* n = 0,
                           FunctionSideEffectInterface* n1 = 0) 
     : funcname(fn), prev(n), prev1(n1) {}
  virtual ~ArrayUseAccessFunction() {}
  virtual bool IsArrayAccess( AstInterface& fa,
                                 const AstNodePtr& s, AstNodePtr* array = 0,
                                 AstInterface::AstNodeList* index = 0) ;
  virtual bool GetArrayBound( AstInterface& fa,
                                 const AstNodePtr& array,
                                 int dim, int &lb, int &ub);
  virtual AstNodePtr CreateArrayAccess( AstInterface& fa, const AstNodePtr& arr,
                                AstInterface::AstNodeList& index);

 // returns false if unknown function encountered
  virtual bool get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0);
  virtual bool get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0);
};

class LoopTransformInterface 
{
  static int configIndex;
  AliasAnalysisInterface& aliasInfo;
  FunctionSideEffectInterface* funcInfo;
  ArrayAbstractionInterface* arrayInfo;
  AstInterface& fa;
 public:
 //LoopTransformInterface la (fa,array_interface,array_annot, &array_interface);
  LoopTransformInterface( AstInterface& _fa, AliasAnalysisInterface& alias,
                          FunctionSideEffectInterface* func = 0,
                          ArrayAbstractionInterface* array = 0) 
    : aliasInfo(alias), funcInfo(func), arrayInfo(array), fa(_fa) {} 
  virtual ~LoopTransformInterface() {}

  operator AstInterface&() { return fa; }
  AstInterface& getAstInterface() { return fa;}
  AliasAnalysisInterface& getAliasInterface() { return aliasInfo; }
  FunctionSideEffectInterface* getSideEffectInterface() { return funcInfo; }
  //! Check if two references are aliased, relying on aliasInfo
  bool IsAliasedRef( const AstNodePtr& r1, const AstNodePtr& r2)
    { return aliasInfo.may_alias(fa, r1, r2); }
  //! Get modified and read variables by function call 'fc', relying on funcInfo  
  bool GetFunctionCallSideEffect( const AstNodePtr& fc,
                     CollectObject<AstNodePtr>& collectmod,
                     CollectObject<AstNodePtr>& collectread);
  //! Check if a node is representing a memory access, relying on either arrayInfo or astInterface
  bool IsMemoryAccess( const AstNodePtr& s)
   { return (arrayInfo != 0 && arrayInfo->IsArrayAccess(fa, s)) ||
            fa.IsMemoryAccess(s); }
            
  bool IsLoop( const AstNodePtr& s, 
                       SymbolicVal* init = 0, SymbolicVal* cond=0,
                       SymbolicVal* incr =0, AstNodePtr* body=0);
  bool IsFortranLoop( const AstNodePtr& s, SymbolicVar* ivar = 0,
                       SymbolicVal* lb = 0, SymbolicVal* ub=0,
                       SymbolicVal* step =0, AstNodePtr* body=0);
  //! Check if a node is representing an array access, relying on either arrayInfo or astInterface
  bool IsArrayAccess( const AstNodePtr& s, AstNodePtr* array = 0,
                                   AstInterface::AstNodeList* index = 0)  
   { return (arrayInfo != 0 && arrayInfo->IsArrayAccess(fa, s, array, index)) ||
            fa.IsArrayAccess(s, array, index); }
  //! Create an array access, using arrayInfo by default, astInterface otherwise            
  virtual AstNodePtr CreateArrayAccess( const AstNodePtr& arr,
                                AstInterface::AstNodeList& index) 
  { 
    AstNodePtr r = (arrayInfo == 0)? AST_NULL : arrayInfo->CreateArrayAccess(fa,arr,index);
    if (r == AST_NULL)
       r = fa.CreateArrayAccess(arr, index);
    return r;
  }
  //bool IsArrayType( AstInterface& fa, const AstNodeType& t)
  //  { return (arrayInfo != 0 && arrayInfo->IsArrayType(fa, t)) ||
  //          AstInterface::IsArrayType(t); }
  //! Get array bounds, using arrayInfo by default. astInterface is used otherwise
  bool GetArrayBound( const AstNodePtr& array, int dim, int &lb, int &ub) 
   {   
       if (arrayInfo != 0)
           return arrayInfo->GetArrayBound(fa, array, dim, lb, ub);
       return fa.GetArrayBound(array, dim, lb, ub);
   }

  //static AstNodePtr GetLoopIvar( const AstNodePtr& s);
  //static AstNodePtr GetLoopLB( const AstNodePtr& s) ;
  //static AstNodePtr GetLoopUB( const AstNodePtr& s) ;
  //static AstNodePtr GetLoopStep( const AstNodePtr& s) ;
  //static AstNodePtr GetLoopBody( const AstNodePtr& s);

  AstNodePtr CreateDynamicFusionConfig( const AstNodePtr& groupNum, 
                                        AstInterface::AstNodeList& args, int &id);
  AstNodePtr CreateDynamicFusionEnd( int id);
  bool IsDynamicFusionConfig( const AstNodePtr& n, AstNodePtr* configvar = 0,
                                 int* configID = 0, AstInterface::AstNodeList* params = 0);
  bool IsDynamicFusionEnd(const AstNodePtr& n);
};

class SgGlobal;
void SetLoopTransformOptions( std::vector<std::string>& argvList);
void PrintLoopTransformUsage( std::ostream& out);
AstNodePtr LoopTransformTraverse( AstInterface& fa, const AstNodePtr& head, 
                                 AliasAnalysisInterface& aliasInfo,
                                 FunctionSideEffectInterface* funcInfo=0,
                                 ArrayAbstractionInterface* arrayInfo = 0);
/*
Loop normalization, documented by Liao, May 15,2008 
Try to normalize all loops under 'head' node
test expression:
  i<x is normalized to i<= (x-1)
  i>x is normalized to i>= (x+1)
increment expression: 
  i++ is normalized to i=i+1
  i-- is normalized to i=i-1
*/
void NormalizeForLoop (AstInterface& fa, const AstNodePtr& head) ;


#endif
