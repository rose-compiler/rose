#ifndef LOOP_TRANSFORMATION_INTERFACE_H
#define LOOP_TRANSFORMATION_INTERFACE_H

#include <list>
#include <string>
#include <iostream>
#include "AstInterface.h"
#include "SymbolicVal.h"
#include "AnalysisInterface.h"

/*************
QY: Interface classes used by loop optimizations
**************/
class ArrayAbstractionInterface {
 public:
  virtual bool IsArrayAccess( AstInterface& fa,
                                 const AstNodePtr& s, AstNodePtr* array = 0,
                                 AstInterface::AstNodeList* index = 0) = 0 ;
  virtual bool GetArrayBound( AstInterface& fa,
                                 const AstNodePtr& array, 
                                 int dim, int &lb, int &ub) = 0;
  virtual AstNodePtr CreateArrayAccess( AstInterface& fa, const AstNodePtr& arr,
                                AstInterface::AstNodeList& index) = 0;
  virtual SymbolicVal CreateArrayAccess(
                                const SymbolicVal& arr,
                                const SymbolicVal& index) 
     { /*QY: need to be defined by a derived class*/ assert(0); }
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
  virtual SymbolicVal CreateArrayAccess(
                                const SymbolicVal& arr,
                                const SymbolicVal& index) ;
 // returns false if unknown function encountered
  virtual bool get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0);
  virtual bool get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0);

};

class AutoTuningInterface ;
/**********
QY: Singular interface class which remembers configurations for loop 
    optimizations
***********/
class LoopTransformInterface 
{
  static AstInterface* fa;
  static int configIndex;
  static AliasAnalysisInterface* aliasInfo;
  static FunctionSideEffectInterface* funcInfo;
  static ArrayAbstractionInterface* arrayInfo;
  static AutoTuningInterface* tuning;

 public:

  static void set_aliasInfo(AliasAnalysisInterface* alias)
    { aliasInfo= alias; }
  static void set_sideEffectInfo(FunctionSideEffectInterface* func)
    { funcInfo = func; }
  static void set_arrayInfo( ArrayAbstractionInterface* array)
    { arrayInfo = array; }
  static void set_astInterface( AstInterface& _fa);
  static void set_tuningInterface(AutoTuningInterface* _tuning);
  static void cmdline_configure(std::vector<std::string>& argv);

  static AstInterface& getAstInterface() { assert(fa != 0); return *fa; }
  static AliasAnalysisInterface* getAliasInfo() { return aliasInfo; }
  static FunctionSideEffectInterface* getSideEffectInterface() 
            { return funcInfo; }
  static AutoTuningInterface* getAutoTuningInterface() { return tuning; }

  static bool IsAliasedRef( const AstNodePtr& r1, const AstNodePtr& r2)
    { assert(fa != 0 && aliasInfo!=0); return aliasInfo->may_alias(*fa, r1, r2); }
  static bool GetFunctionCallSideEffect( const AstNodePtr& fc,
                     CollectObject<AstNodePtr>& collectmod,
                     CollectObject<AstNodePtr>& collectread);

  static bool IsMemoryAccess( const AstNodePtr& s)
   { assert(fa != 0);
     return (arrayInfo != 0 && arrayInfo->IsArrayAccess(*fa, s)) ||
            fa->IsMemoryAccess(s); }
  static bool IsLoop( const AstNodePtr& s, 
                       SymbolicVal* init = 0, SymbolicVal* cond=0,
                       SymbolicVal* incr =0, AstNodePtr* body=0);
  static bool IsArrayAccess( const AstNodePtr& s, AstNodePtr* array = 0,
                                   AstInterface::AstNodeList* index = 0)  
   { assert(fa != 0);
     return (arrayInfo != 0 && arrayInfo->IsArrayAccess(*fa, s, array, index)) 
            || fa->IsArrayAccess(s, array, index); }

  static AstNodePtr CreateArrayAccess( const AstNodePtr& arr,
                                AstInterface::AstNodeList& index) 
  { assert(fa != 0);
    AstNodePtr r = (arrayInfo == 0)? AST_NULL : arrayInfo->CreateArrayAccess(*fa,arr,index);
    if (r == AST_NULL)
       r = fa->CreateArrayAccess(arr, index);
    return r;
  }
  static AstNodePtr CreateArrayAccess(const std::string& arrname, 
                            const std::vector<SymbolicVal>& arrindex);
  static bool GetArrayBound( const AstNodePtr& array,int dim, int &lb, int &ub) 
  { assert(fa!=0);  
    if (arrayInfo != 0 && arrayInfo->IsArrayAccess(*fa, array))
      return arrayInfo->GetArrayBound(*fa, array, dim, lb, ub);
    return fa->GetArrayBound(array, dim, lb, ub);
  }

  static AstNodePtr CreateDynamicFusionConfig( const AstNodePtr& groupNum, 
                                        AstInterface::AstNodeList& args, int &id);
  static AstNodePtr CreateDynamicFusionEnd( int id);
  static bool IsDynamicFusionConfig( const AstNodePtr& n, AstNodePtr* configvar = 0,
                                 int* configID = 0, AstInterface::AstNodeList* params = 0);
  static bool IsDynamicFusionEnd(const AstNodePtr& n);

  /*************
   QY: apply transformations to head; return the transformation result
  ************/
  static AstNodePtr 
  TransformTraverse( AstInterfaceImpl& scope, const AstNodePtr& head);

  static void PrintTransformUsage( std::ostream& out);
};


/* normalize the forloops in C
  i<x is normalized to i<= (x-1)
  i>x is normalized to i>= (x+1)

  i++ is normalized to i=i+1
  i-- is normalized to i=i-1
*/
void NormalizeForLoop (AstInterface& fa, const AstNodePtr& head) ;


#endif
